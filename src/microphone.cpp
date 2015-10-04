#include <microphone.hpp>

Microphone::Microphone(QWidget *parent) : QWidget(parent)
{
    AudioRecorder = new QAudioRecorder(this);

    AudioRecorder->setAudioInput("Default");

    QAudioEncoderSettings RecordSettings;
    RecordSettings.setCodec("audio/PCM");
    RecordSettings.setSampleRate(44100);
    //RecordSettings.setBitRate(32);
    RecordSettings.setChannelCount(2);
    RecordSettings.setQuality(QMultimedia::HighQuality);
    RecordSettings.setEncodingMode(QMultimedia::ConstantQualityEncoding);
    AudioRecorder->setEncodingSettings(RecordSettings, QVideoEncoderSettings(), "wav");

    Layout = new QGridLayout(this);
    //layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(Layout);
    createUi();

}

void Microphone::createUi() {

    ButtonRecord = new QPushButton("Record");
    connect(ButtonRecord, SIGNAL(clicked()), this, SLOT(startRecord()));
    Layout->addWidget(ButtonRecord, 0, 0);
    ButtonPause = new QPushButton("Pause");
    ButtonPause->setEnabled(false);
    Layout->addWidget(ButtonPause, 1, 0);

}

void Microphone::startRecord() {
    disconnect(ButtonRecord, SIGNAL(clicked()), this, SLOT(startRecord()));
    connect(ButtonRecord, SIGNAL(clicked()), this, SLOT(stopRecord()));
    ButtonRecord->setText("Stop");

    qDebug() << QDir::currentPath();
    foreach (auto var, AudioRecorder->audioInputs()) {
        qDebug() << var;
    }
    AudioRecorder->setOutputLocation(QUrl::fromLocalFile(QDir::currentPath() + "//" + "test" + ".wav"));
    AudioRecorder->record();

}

void Microphone::stopRecord() {
    qDebug() << AudioRecorder->outputLocation();
    disconnect(ButtonRecord, SIGNAL(clicked()), this, SLOT(stopRecord()));
    connect(ButtonRecord, SIGNAL(clicked()), this, SLOT(startRecord()));
    ButtonRecord->setText("Record");
    AudioRecorder->stop();
}

Microphone::~Microphone(){

}
