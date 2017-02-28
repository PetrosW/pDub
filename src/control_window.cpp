#include <windows/control_window.hpp>

Window_Control_t::Window_Control_t(QWidget *parent) : QWidget(parent), Window_Editor_Ptr(nullptr), Window_Video_Ptr(nullptr)
{

    NextRecordId = 0;
    setWindowFlags(Qt::Tool);
    //nahození za jízdy, čili za show funguje
    /*QPushButton *button = new QPushButton("Hello");
    layout->addWidget(button);*/
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

    ButtonDockWindowVideo = new QPushButton(this);
    ControlLayout->addWidget(ButtonDockWindowVideo, 0, 0);

}


//public slots

void Window_Control_t::videoStopEnd() {
    VideoStop();
}

//private slots



