#include <windows/control_window.hpp>

Window_Control_t::Window_Control_t(QWidget *parent) : QWidget(parent), Window_Editor_Ptr(nullptr), Window_Video_Ptr(nullptr)
{
    NextRecordId = 0;
}

//public

void Window_Control_t::setWindowEditorPtr(Window_Editor_t *Window_Editor)
{
    this->Window_Editor_Ptr = Window_Editor;
}

void Window_Control_t::setWindowVideoPtr(Window_Video_t *Window_Video)
{
    this->Window_Video_Ptr = Window_Video;
}

void Window_Control_t::setDeafaultMicrophone(){
    newMicrophone();
}

//private

void Window_Control_t::newMicrophone() {
    Microphone *mic1 = new Microphone(this, Window_Video_Ptr, this);
    connect(mic1, &Microphone::recordingEnd, Window_Editor_Ptr, &Window_Editor_t::addNewRecordObject);
    connect(this, &Window_Control_t::VideoStop, mic1, &Microphone::videoStopEnd);
    Layout->addWidget(mic1, 0, 1);
    Layout->setColumnMinimumWidth(1, 100);
}

void Window_Control_t::createUi() {
    setWindowTitle("pDub");
    Layout = new QGridLayout();
    //Layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    Layout->setColumnMinimumWidth(0, 100);

    setLayout(Layout);

    ControlLayout = new QGridLayout();
    //ControlLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    Layout->addLayout(ControlLayout, 0, 0);

    ButtonDockWindowVideo = new QPushButton("UnDock Video", this);
    ButtonDockWindowVideo->setMaximumWidth(200);
    ControlLayout->addWidget(ButtonDockWindowVideo, 0, 0);

}

void Window_Control_t::createAudioEngine(QMap<quint32, QMap<quint32, Record *> > *Records_Map)
{
    AudioPlayback = new AudioPlayback_t(Records_Map, RecordPath(), this);
    AudioPlayback->init();
    //AudioPlayback->planUpdate();
    connect(Window_Video_Ptr, &Window_Video_t::signalVideoPlay, AudioPlayback, &AudioPlayback_t::play);
    connect(Window_Video_Ptr, &Window_Video_t::signalVideoPause, AudioPlayback, &AudioPlayback_t::pause);
    connect(Window_Video_Ptr, &Window_Video_t::signalSeekFinished, AudioPlayback, &AudioPlayback_t::seek);
    connect(this, &Window_Control_t::RecordPathChanged, this, &Window_Control_t::updateAudioEngineFilePath);
}

void Window_Control_t::updateAudioEngineFilePath()
{
    AudioPlayback->changeFilePath(RecordPath() );
}

void Window_Control_t::updateAudioEngine()
{
    AudioPlayback->planUpdate();
}

void Window_Control_t::releaseAudioResources()
{
    AudioPlayback->planClear();
}

//public slots

void Window_Control_t::videoStopEnd() {
    VideoStop();
}

//private slots



