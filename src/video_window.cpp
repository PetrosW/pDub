#include <windows/video_window.hpp>

Window_Video_t::Window_Video_t(Window_Control_t *Window_Control, QWidget *Window_Control_QWidget)
    : QWidget(Window_Control_QWidget),
      Window_Control_Ptr(Window_Control),
      Window_Editor_Ptr(nullptr)
{
    QtAV::Widgets::registerRenderers();
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint);
    setWindowTitle("Video");


    Layout = new QGridLayout(this);
    setLayout(Layout);

    VideoOutput = new QtAV::VideoOutput(this);
    if (!VideoOutput->widget()) {
        QMessageBox::warning(0, QString::fromLatin1("QtAV error"), tr("Can not create video renderer"));
        return;
    }
    Player = new QtAV::AVPlayer(this);
    Player->setRenderer(VideoOutput);
    Layout->addWidget(VideoOutput->widget(), 0, 0);

    createUi();

    show();
}

void Window_Video_t::setWindowEditorPtr(Window_Editor_t *Window_Editor)
{
    Window_Editor_Ptr = Window_Editor;
}

void Window_Video_t::firstPlay(QString FileName) {
    ButtonPlay->setEnabled(true);
    Player->play(FileName);
}

void Window_Video_t::play() {
    ButtonPlay->setText("Pause");
    disconnect(ButtonPlay, SIGNAL(clicked()), this, SLOT(play()));
    connect(ButtonPlay, SIGNAL(clicked()), this, SLOT(pause()));
    Player->pause(false);
}

void Window_Video_t::pause() {
    ButtonPlay->setText("Play");
    disconnect(ButtonPlay, SIGNAL(clicked()), this, SLOT(pause()));
    connect(ButtonPlay, SIGNAL(clicked()), this, SLOT(play()));
    Player->pause(true);
}

void Window_Video_t::createUi() {
    ButtonPlay = new QPushButton("Pause");
    connect(ButtonPlay, SIGNAL(clicked()), this, SLOT(pause()));
    ButtonPlay->setEnabled(false);

    Layout->addWidget(ButtonPlay, 1, 0);
}
