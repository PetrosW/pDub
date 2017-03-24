#ifndef MIXING_DEVICE_HPP
#define MIXING_DEVICE_HPP

#include <valarray>
#include <vector>

#include <QFile>
#include <QIODevice>
#include <QMap>

#include <record.hpp>

const double OneSampleInMs = (1.0 / 44100.0) * 1000;

class PlannedRecord_t
{
    public:
        PlannedRecord_t(Record *Track, QString &FilePath)
            : RecordPtr(Track), WavFile(FilePath + '/' + Track->Name() ), StartingSample(floor(Track->StartTime() / OneSampleInMs) ),
              EndingSample(floor(Track->EndTime() / OneSampleInMs) ) {}
        PlannedRecord_t(PlannedRecord_t &&rhs)
            : RecordPtr(rhs.RecordPtr), WavFile(rhs.WavFile.fileName() ), StartingSample(rhs.StartingSample), EndingSample(rhs.EndingSample) {}

        Record *RecordPtr;
        QFile WavFile;
        quint64 StartingSample;
        quint64 EndingSample;
};

class MixingDevice_t : public QIODevice
{
    Q_OBJECT

    public:
        MixingDevice_t(QMap<quint32, QMap<quint32, Record *> > *Records_Map, QString &FilePath);
        void seek(quint64 SamplePos);
        void planUpdate();
        void planClear();
        void changeFilePath(QString &FilePath);

    protected:
        qint64 readData(char *Data, qint64 Maxlen);
        qint64 writeData(const char *, qint64);

    private:
        QString FilePath;
        QMap<quint32, QMap<quint32, Record *> > *Records_Map;
        std::vector<PlannedRecord_t> Records_Plan;
        std::vector<PlannedRecord_t *> Records_New;
        QMap<quint32, PlannedRecord_t *> Records_Active;
        std::vector<quint32> Records_Finished;
        quint64 SamplePosition;
        quint64 BufferSize;
        std::valarray<qint16> Buffer_Result;
        std::valarray<qint16> Buffer_Tmp;
        quint8 *Buffer_ResultPtr;
        char *Buffer_TmpPtr;
        qint16 Records_PlanIndex;

    signals:
        void initStartComplete();
};

#endif
