#include <microphone.hpp>

Microphone::Microphone(Window_Control_t *Window_Control_ptr, Window_Video_t *Window_Video_ptr, QWidget *parent)
     : QWidget(parent),
       Window_Control_Ptr(Window_Control_ptr),
       Window_Video_Ptr(Window_Video_ptr)
{
    AudioRecorder = new QAudioRecorder(this);
    AudioRecorder->setAudioInput("Default");

    QAudioEncoderSettings RecordSettings;
    RecordSettings.setCodec("audio/PCM");
    RecordSettings.setSampleRate(44100);
    //RecordSettings.setBitRate(32);
    RecordSettings.setChannelCount(2);
    //RecordSettings.setQuality(QMultimedia::HighQuality);
    RecordSettings.setEncodingMode(QMultimedia::ConstantQualityEncoding);
    AudioRecorder->setEncodingSettings(RecordSettings, QVideoEncoderSettings(), "wav");

    DurationTime = 0;
    TimerRecord = new QTimer(this);
    connect(TimerRecord, &QTimer::timeout, this, &Microphone::timerRecordTick);

    Layout = new QGridLayout();
    this->setMaximumWidth(200);
    //layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(Layout);
    createUi();

}

void Microphone::createUi() {
    LabelStartTime = new QLabel(this);
    LabelStartTime->setText("Start time: 00:00:00");
    Layout->addWidget(LabelStartTime, 0, 0);
    LabelEndTime = new QLabel(this);
    LabelEndTime->setText("End time: 00:00:00");
    Layout->addWidget(LabelEndTime, 1, 0);
    LabelDurationTime = new QLabel(this);
    LabelDurationTime->setText("Duration time: 00:00:00");
    Layout->addWidget(LabelDurationTime, 2, 0);


    ButtonRecord = new QPushButton("Record", this);
    connect(ButtonRecord, &QPushButton::clicked, this, &Microphone::startRecord);
    Layout->addWidget(ButtonRecord, 3, 0);
    ButtonPause = new QPushButton("Pause", this);
    ButtonPause->setEnabled(false);
    Layout->addWidget(ButtonPause, 4, 0);

}

//public slots

void Microphone::videoStopEnd() {
    if (AudioRecorder->state() == QAudioRecorder::RecordingState) {
        stopRecord();
    }
}

//private slots

void Microphone::startRecord() {
    if (Window_Video_Ptr->isPaused() == true) {
        Window_Video_Ptr->play();
    }
    Window_Video_Ptr->setMute(true);
    AudioRecorder->setOutputLocation(QUrl::fromLocalFile(Window_Control_Ptr->RecordPath() + "//" + "record" + QString::number(Window_Control_Ptr->NextRecordId) + ".wav"));
    AudioRecorder->record();
    qDebug() << "startTimeMicro1: "<< StartTime;
    StartTime = Window_Video_Ptr->getPlayerPosition();
    qDebug() << "startTimeMicro2: "<< StartTime;
    LabelStartTime->setText(QString("Start time: %1").arg(miliSecToTime(StartTime)));
    DurationTime = 0;
    TimerRecord->start(100);
    disconnect(ButtonRecord, &QPushButton::clicked, this, &Microphone::startRecord);
    connect(ButtonRecord, &QPushButton::clicked, this, &Microphone::stopRecord);
    ButtonRecord->setText("Stop");
}

void Microphone::stopRecord() {
    Window_Video_Ptr->setMute(false);
    AudioRecorder->stop();
    EndTime = Window_Video_Ptr->getPlayerPosition();
    LabelEndTime->setText(QString("End time: %1").arg(miliSecToTime(EndTime)));
    TimerRecord->stop();
    LabelDurationTime->setText(QString("Duration time: %1").arg(miliSecToTime(EndTime - StartTime)));
    disconnect(ButtonRecord, &QPushButton::clicked, this, &Microphone::stopRecord);
    connect(ButtonRecord, &QPushButton::clicked, this, &Microphone::startRecord);
    ButtonRecord->setText("Record");
    qDebug() << "startTimeMicro3: "<< StartTime;
    recordingEnd(Window_Control_Ptr->NextRecordId, StartTime, 0, "record" + QString::number(Window_Control_Ptr->NextRecordId) + ".wav", 1); //1 = secodn line
}

void Microphone::timerRecordTick() {
    DurationTime += 100;
    LabelDurationTime->setText(QString("Duration time: %1").arg(miliSecToTime(DurationTime)));
}

Microphone::~Microphone(){

}
