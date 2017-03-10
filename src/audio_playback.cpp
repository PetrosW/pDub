#include <audio_playback/audio_playback.hpp>
#include <QDebug>
#include <QTimer>

AudioPlayback_t::AudioPlayback_t(QMap<quint32, QMap<quint32, Record *> > *Records_Map, QString FilePath, QObject *Parent)
    : QObject(Parent), MixingDevice(Records_Map, FilePath)
{
    connect(&MixingDevice, &MixingDevice_t::initStartComplete, this, &AudioPlayback_t::initStartComplete);
}

void AudioPlayback_t::init()
{
    QAudioFormat AudioFormat;
    AudioFormat.setCodec("audio/pcm");
    AudioFormat.setChannelCount(2);
    AudioFormat.setSampleSize(16);
    AudioFormat.setSampleRate(44100);
    AudioFormat.setSampleType(QAudioFormat::SignedInt);
    AudioFormat.setByteOrder(QAudioFormat::LittleEndian);

    QAudioDeviceInfo DeviceInfo(QAudioDeviceInfo::defaultOutputDevice() );
    if (!DeviceInfo.isFormatSupported(AudioFormat) )
    {
        throw AudioPlaybackErrorCode::PCM_UNSUPPORTED;
        // or return?
    }

    MixingDevice.open(QIODevice::ReadOnly);

    // Default sound output initialized & opened
    AudioOutput = new QAudioOutput(AudioFormat, this);
    AudioOutput->start(&MixingDevice);
}

void AudioPlayback_t::planUpdate()
{
    MixingDevice.planUpdate();
}

void AudioPlayback_t::planClear()
{
    MixingDevice.planClear();
}

void AudioPlayback_t::initStartComplete()
{
    AudioOutput->reset();
    //QTimer::singleShot(0, [this]{AudioOutput->reset();qDebug() << AudioOutput->state();});
}

void AudioPlayback_t::start()
{
    if (AudioOutput->state() == QAudio::SuspendedState) AudioOutput->resume();
    else if (AudioOutput->state() == QAudio::StoppedState) AudioOutput->start(&MixingDevice);
}

void AudioPlayback_t::pause()
{
    AudioOutput->suspend();
}

void AudioPlayback_t::seek(quint64 Miliseconds)
{
    AudioOutput->reset();
    MixingDevice.seek(Miliseconds);
}

void AudioPlayback_t::changeFilePath(QString &FilePath)
{
    MixingDevice.changeFilePath(FilePath);
}
