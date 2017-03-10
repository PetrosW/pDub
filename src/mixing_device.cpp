#include <audio_playback/mixing_device.hpp>
#include <QDebug>

MixingDevice_t::MixingDevice_t(QMap<quint32, QMap<quint32, Record *> > *Records_Map, QString &FilePath)
    : QIODevice(nullptr), FilePath(FilePath), Records_Map(Records_Map), SamplePosition(0), BufferSize(0), Records_PlanIndex(-1)
{}


qint64 MixingDevice_t::writeData(const char *, qint64)
{
    return 0;
}

qint64 MixingDevice_t::readData(char *Data, qint64 Maxlen)
{
    qDebug() << Maxlen;
    if (!BufferSize)
    {
        BufferSize = (Maxlen >> 1);

        Buffer_Result.resize(BufferSize);
        Buffer_Tmp.resize(BufferSize);

        Buffer_ResultPtr = reinterpret_cast<quint8 *>(&Buffer_Result[0]);
        Buffer_TmpPtr = reinterpret_cast<char *>(&Buffer_Tmp[0]);

        emit initStartComplete();
        return 0;
    }

    memset(Buffer_ResultPtr, 0, Maxlen);
    SamplePosition += (Maxlen >> 2);

    // handle new
    if (Records_PlanIndex >= 0)
    {
        qint16 Records_Count = Records_Plan.size();
        while ( (Records_PlanIndex < Records_Count) && (SamplePosition >= Records_Plan[Records_PlanIndex].StartingSample) )
        {
            quint32 ZeroLength = ( (Records_Plan[Records_PlanIndex].StartingSample - (SamplePosition - (Maxlen >> 2) ) ) << 2);
            quint32 DataLength = ( (SamplePosition - Records_Plan[Records_PlanIndex].StartingSample) << 2);

            memset(Buffer_TmpPtr, 0, ZeroLength);
            qint64 BytesRead = Records_Plan[Records_PlanIndex].WavFile.read(Buffer_TmpPtr + ZeroLength, DataLength);

            if (BytesRead < DataLength) memset(Buffer_TmpPtr + ZeroLength + BytesRead, 0, DataLength - BytesRead);
            else Records_New.push_back(&Records_Plan[Records_PlanIndex]);

            Records_PlanIndex++;
            Buffer_Result += Buffer_Tmp;
        }

        if (Records_PlanIndex == Records_Count) Records_PlanIndex = -1;
    }

    // Read and combine samples from active records
    for (auto Track = Records_Active.begin(); Track != Records_Active.end(); Track++)
    {
        qint64 BytesRead = (*Track)->WavFile.read(Buffer_TmpPtr, Maxlen);
        if (BytesRead < Maxlen)
        {
            memset(Buffer_TmpPtr + BytesRead, 0, Maxlen - BytesRead);
            Records_Finished.push_back(Track.key() );
        }

        Buffer_Result += Buffer_Tmp;
    }

    // Remove finished records from active map
    for (auto TrackId: Records_Finished) Records_Active.remove(TrackId);
    Records_Finished.clear();

    // Add new records to active map
    for (auto Track: Records_New) Records_Active.insert(Track->RecordPtr->Id(), Track);
    Records_New.clear();

    memcpy(Data, Buffer_ResultPtr, Maxlen);
    return Maxlen;
}

void MixingDevice_t::seek(quint64 Miliseconds)
{
    // Clear current playing state
    Records_Active.clear();

    SamplePosition = floor(Miliseconds / OneSampleInMs);
    char ChunkName[5];
    char ChunkSize[4];
    quint32 *ChunkSize_Int = reinterpret_cast<quint32 *>(ChunkSize);
    memset(ChunkName, 0, 5); // For \0 at the end

    // Reset all tracks to the beginning
    for (auto Track = Records_Plan.begin(); Track != Records_Plan.end(); Track++)
    {
        Track->WavFile.seek(0);
        *ChunkSize_Int = 12;

        do {
            Track->WavFile.seek(Track->WavFile.pos() + (*ChunkSize_Int) );
            Track->WavFile.read(ChunkName, 4);
            Track->WavFile.read(ChunkSize, 4);
        } while (QString(ChunkName).toLower() != "data");
    }

    // Choose possible active candidates
    std::vector<quint16> PossibleActiveRecords;
    quint16 i;
    for (i = 0; (i < Records_Plan.size() ) && (Records_Plan[i].RecordPtr->StartTime() <= Miliseconds); i++)
        PossibleActiveRecords.push_back(i);

    // -1 if all records were checked
    Records_PlanIndex = (i == Records_Plan.size() ? -1 : i);

    // Add active records to the map and file seek to the correct positions
    for (auto Index: PossibleActiveRecords)
    {
        if (Records_Plan[Index].RecordPtr->EndTime() > Miliseconds)
        {
            Records_Active.insert(Records_Plan[Index].RecordPtr->Id(), &Records_Plan[Index]);
            Records_Plan[Index].WavFile.seek(Records_Plan[Index].WavFile.pos() + ( (SamplePosition - Records_Plan[Index].StartingSample) << 2) );
        }
    }
}

void MixingDevice_t::planClear()
{
    Records_Active.clear();
    Records_Finished.clear();
    Records_New.clear();
    Records_Plan.clear();
}

void MixingDevice_t::planUpdate()
{
    planClear();

    for (auto TracksAtTime: *Records_Map)
    {
        for (auto Record: TracksAtTime ) Records_Plan.emplace_back(Record, FilePath);
    }

    // Magic...
    for (auto Track = Records_Plan.begin(); Track != Records_Plan.end(); Track++) Track->WavFile.open(QIODevice::ReadOnly);
    seek(SamplePosition);
}

void MixingDevice_t::changeFilePath(QString &FilePath)
{
    this->FilePath = FilePath;
}
