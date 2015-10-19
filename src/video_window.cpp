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
    Player->audio()->setVolume(0.8);
    Player->setSeekType(QtAV::AccurateSeek);
    connect(Player, &QtAV::AVPlayer::started, this, &Window_Video_t::playInit);
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

void Window_Video_t::createUi() {

    SliderVideoTime = new QSlider(Qt::Horizontal, this);
    SliderVideoTime->setMinimum(0);

    SliderVideoTime->setTickInterval(1);
    connect(SliderVideoTime, SIGNAL(sliderMoved(int)), this, SLOT(updateVideoTimePositionSliderMove(int)));
    connect(Player, SIGNAL(positionChanged(qint64)), this, SLOT(updateSilderTimeValue(qint64)));

    Layout->addWidget(SliderVideoTime, 1, 0);

    LayoutVideoControl = new QGridLayout();

    Layout->addLayout(LayoutVideoControl, 2, 0);

    LabelVideoTime = new QLabel("00:00:00.00", this);
    LabelVideoTime->setFont(QFont("Times", 14));
    LabelVideoTime->setMaximumWidth(100);
    LayoutVideoControl->addWidget(LabelVideoTime, 0, 0);

    ButtonSeekBackward = new QPushButton("<<", this);
    ButtonSeekBackward->setMaximumWidth(50);
    connect(ButtonSeekBackward, SIGNAL(pressed()), this, SLOT(seekBackward()));
    LayoutVideoControl->addWidget(ButtonSeekBackward, 0, 1);

    ButtonPlay = new QPushButton("Play", this);
    connect(ButtonPlay, SIGNAL(clicked()), this, SLOT(play()));
    ButtonPlay->setEnabled(false);
    LayoutVideoControl->addWidget(ButtonPlay, 0, 2);

    ButtonSeekForward = new QPushButton(">>", this);
    ButtonSeekForward->setMaximumWidth(50);
    connect(ButtonSeekForward, SIGNAL(pressed()), this, SLOT(seekForward()));
    LayoutVideoControl->addWidget(ButtonSeekForward, 0, 3);

    SliderVideoVolume = new QSlider(Qt::Horizontal, this);
    SliderVideoVolume->setMaximumWidth(100);
    SliderVideoVolume->setMinimum(0);
    SliderVideoVolume->setMaximum(100);
    SliderVideoVolume->setTickInterval(1);
    SliderVideoVolume->setValue(80);
    connect(SliderVideoVolume, SIGNAL(valueChanged(int)), this, SLOT(setVolume(int)));
    LayoutVideoControl->addWidget(SliderVideoVolume, 0, 4);

}

int Window_Video_t::getPlayerPosition() {
    return int(Player->position());
}

// private slots
void Window_Video_t::playInit() {
    SliderVideoTime->setMaximum(Player->duration());
    Window_Editor_Ptr->setAfterVideoLoad(Player->duration());
    while (!Player->isPlaying() || !Player->isPaused()) {
        if (Player->isPlaying()) {
            Player->setPosition(qint64(0));
            Player->pause(true);
        }
    }
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

void Window_Video_t::setVolume(int newVolume) {
    Player->audio()->setVolume(newVolume/100.0);
}

void Window_Video_t::updateVideoTimePositionSliderMove(int newPlayerTimePosition) {
    Player->seek(qint64(newPlayerTimePosition));
}

void Window_Video_t::updateVideoTimePositionSliderPressed() {
    Player->seek(qint64(SliderVideoTime->value()));
}

void Window_Video_t::updateSilderTimeValue(qint64 newSliderPosition) {
    SliderVideoTime->setValue(newSliderPosition);
    LabelVideoTime->setText(miliSecToTime(newSliderPosition));
}

void Window_Video_t::seekBackward() {
    Player->seek(Player->position() - 1000);
}

void Window_Video_t::seekForward() {
    Player->seek(Player->position() + 1000);
}
