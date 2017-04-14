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
//    qDebug() << "Maxlen: " << Maxlen;
    if (!BufferSize)
    {
        BufferSize = Maxlen;
        Buffer_TmpVector.reserve(Maxlen);
        Buffer_ResultVector.reserve(Maxlen);

        Buffer_Result = &Buffer_ResultVector.front();
        Buffer_Tmp = &Buffer_TmpVector.front();

        Buffer_ResultSampleFormat = reinterpret_cast<qint16 *>(Buffer_Result);
        Buffer_TmpSampleFormat = reinterpret_cast<qint16 *>(Buffer_Tmp);

        emit initStartComplete();
        return 0;
    }

    qint32 Maxlen_Samples = (Maxlen >> 1);

    memset(Buffer_Result, 0, Maxlen);
    SamplePosition += (Maxlen >> 2);

    // handle new
    if (Records_PlanIndex >= 0)
    {
        qint16 Records_Count = Records_Plan.size();
        while ( (Records_PlanIndex < Records_Count) && (SamplePosition >= Records_Plan[Records_PlanIndex].StartingSample) )
        {
            quint32 ZeroLength = ( (Records_Plan[Records_PlanIndex].StartingSample - (SamplePosition - (Maxlen >> 2) ) ) << 2);
            quint32 DataLength = ( (SamplePosition - Records_Plan[Records_PlanIndex].StartingSample) << 2);

            memset(Buffer_Tmp, 0, ZeroLength);
            qint64 BytesRead = Records_Plan[Records_PlanIndex].WavFile.read(Buffer_Tmp + ZeroLength, DataLength);

            if (BytesRead < DataLength) memset(Buffer_Tmp + ZeroLength + BytesRead, 0, DataLength - BytesRead);
            else Records_New.push_back(&Records_Plan[Records_PlanIndex]);

            if ( !(Records_Plan[Records_PlanIndex].RecordPtr->isMuted() ) )
            {
                float Volume = Records_Plan[Records_PlanIndex].RecordPtr->VolumeNormalized();

                for (qint32 i = 0; i < Maxlen_Samples; i++)
                {
                    qint32 ProcessedSample = (Buffer_TmpSampleFormat[i] * Volume) + Buffer_ResultSampleFormat[i];

                    if (ProcessedSample < float(INT16_MIN) ) Buffer_ResultSampleFormat[i] = INT16_MIN;
                    else if (ProcessedSample > float(INT16_MAX) ) Buffer_ResultSampleFormat[i] = INT16_MAX;
                    else Buffer_ResultSampleFormat[i] = ProcessedSample;
                }
            }
            Records_PlanIndex++;
        }

        if (Records_PlanIndex == Records_Count) Records_PlanIndex = -1;
    }

    // Read and combine samples from active records
    for (auto Track = Records_Active.begin(); Track != Records_Active.end(); Track++)
    {
        qint64 BytesRead = (*Track)->WavFile.read(Buffer_Tmp, Maxlen);
        if (BytesRead < Maxlen)
        {
            memset(Buffer_Tmp + BytesRead, 0, Maxlen - BytesRead);
            Records_Finished.push_back(Track.key() );
        }

        if ( !( (*Track)->RecordPtr->isMuted() ) )
        {
            float Volume = (*Track)->RecordPtr->VolumeNormalized();

            for (qint32 i = 0; i < Maxlen_Samples; i++)
            {
                qint32 ProcessedSample = (Buffer_TmpSampleFormat[i] * Volume) + Buffer_ResultSampleFormat[i];

                if (ProcessedSample < float(INT16_MIN) ) Buffer_ResultSampleFormat[i] = INT16_MIN;
                else if (ProcessedSample > float(INT16_MAX) ) Buffer_ResultSampleFormat[i] = INT16_MAX;
                else Buffer_ResultSampleFormat[i] = ProcessedSample;
            }
        }
    }

    // Remove finished records from active map
    for (auto TrackId: Records_Finished) Records_Active.remove(TrackId);
    Records_Finished.clear();

    // Add new records to active map
    for (auto Track: Records_New) Records_Active.insert(Track->RecordPtr->Id(), Track);
    Records_New.clear();

    memcpy(Data, Buffer_Result, Maxlen);
    return Maxlen;
}

void MixingDevice_t::seek(quint64 SamplePos)
{
    // Clear current playing state
    Records_Active.clear();

    SamplePosition = SamplePos;
    //qDebug() << "SamplePosition: " << SamplePosition;
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
    for (i = 0; (i < Records_Plan.size() ) && (Records_Plan[i].StartingSample <= SamplePosition); i++)
        PossibleActiveRecords.push_back(i);

    // -1 if all records were checked
    Records_PlanIndex = (i == Records_Plan.size() ? -1 : i);

    // Add active records to the map and file seek to the correct positions
    for (auto Index: PossibleActiveRecords)
    {
        if (Records_Plan[Index].EndingSample > SamplePosition)
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
