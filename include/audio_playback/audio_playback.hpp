#ifndef AUDIO_PLAYBACK_HPP
#define AUDIO_PLAYBACK_HPP

#include <QAudioOutput>
#include <QIODevice>
#include <QMap>

#include <record.hpp>
#include <audio_playback/mixing_device.hpp>
#include <audio_playback/error_codes.hpp>

class AudioPlayback_t : public QObject
{
    Q_OBJECT

    public:
        AudioPlayback_t(QMap<quint32, QMap<quint32, Record *> > *Records_Map, QString FilePath, QObject *Parent = nullptr);
        void init();
        void planUpdate();
        void planClear();
        //void changeFilePath(QString &FilePath);

    private:
        QAudioOutput *AudioOutput;
        MixingDevice_t MixingDevice;

    public slots:
        void play();
        void pause();
        void seek(quint64 Miliseconds);
        void changeFilePath(QString FilePath);

    private slots:
        void initStartComplete();


};

#endif
