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
    connect(TimerRecord, SIGNAL(timeout()), this, SLOT(timerRecordTick()));


    Layout = new QGridLayout(this);
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
    connect(ButtonRecord, SIGNAL(clicked()), this, SLOT(startRecord()));
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
//    qDebug() << Window_Video_Ptr->getPlayerPosition();
    if (Window_Video_Ptr->isPaused() == true) {
        Window_Video_Ptr->play();
    }
    Window_Video_Ptr->setMute(true);
    AudioRecorder->setOutputLocation(QUrl::fromLocalFile(Window_Control_Ptr->RecordPath() + "//" + "record" + QString::number(Window_Control_Ptr->NextRecordId) + ".wav"));
    AudioRecorder->record();



    StartTime = Window_Video_Ptr->getPlayerPosition();
    LabelStartTime->setText(QString("Start time: %1").arg(miliSecToTime(StartTime)));
    DurationTime = 0;
    TimerRecord->start(100);
    disconnect(ButtonRecord, SIGNAL(clicked()), this, SLOT(startRecord()));
    connect(ButtonRecord, SIGNAL(clicked()), this, SLOT(stopRecord()));
    ButtonRecord->setText("Stop");

}

void Microphone::stopRecord() {
//    qDebug() << Window_Video_Ptr->getPlayerPosition();
    Window_Video_Ptr->setMute(false);
    AudioRecorder->stop();
    LabelEndTime->setText(QString("End time: %1").arg(miliSecToTime(EndTime)));
    TimerRecord->stop();
    LabelDurationTime->setText(QString("Duration time: %1").arg(miliSecToTime(EndTime - StartTime)));
    disconnect(ButtonRecord, SIGNAL(clicked()), this, SLOT(stopRecord()));
    connect(ButtonRecord, SIGNAL(clicked()), this, SLOT(startRecord()));
    ButtonRecord->setText("Record");
    recordingEnd(Window_Control_Ptr->NextRecordId, StartTime, 0, "record" + QString::number(Window_Control_Ptr->NextRecordId) + ".wav", 500); //500 kvůli řadě

}

void Microphone::timerRecordTick() {
    DurationTime += 100;
    LabelDurationTime->setText(QString("Duration time: %1").arg(miliSecToTime(DurationTime)));
}

Microphone::~Microphone(){

}
