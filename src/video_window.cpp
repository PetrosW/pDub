#include <windows/video_window.hpp>

Window_Video_t::Window_Video_t(QWidget *parent) : QWidget(parent), Window_Control_Ptr(nullptr), Window_Editor_Ptr(nullptr)
{
    QtAV::Widgets::registerRenderers();
    //setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint);
    //setWindowTitle("Video");

    qDebug() << "video pre";

    Layout = new QGridLayout();
    setLayout(Layout);

    VideoOutput = new QtAV::VideoOutput(this);
    if (!VideoOutput->widget()) {
        QMessageBox::warning(0, QString::fromLatin1("QtAV error"), tr("Can not create video renderer"));
        return;
    }
    Player = new QtAV::AVPlayer(this);
    Player->setRenderer(VideoOutput);
    Player->setSeekType(QtAV::AccurateSeek);

    connect(Player, &QtAV::AVPlayer::started, this, &Window_Video_t::playInit);
    Layout->addWidget(VideoOutput->widget(), 0, 0);

    sliderEditorSeek = false;
    isPlayingSliderPress = false;
    isPlaying = false;
    IsFullScreen = false;
}

void Window_Video_t::setWindowEditorPtr(Window_Editor_t *Window_Editor)
{
    Window_Editor_Ptr = Window_Editor;
}

void Window_Video_t::setWindowControlPtr(Window_Control_t *Window_Control)
{
    Window_Control_Ptr = Window_Control;
}

void Window_Video_t::createUi() {

    SliderVideoTime = new QSlider(Qt::Horizontal, this);
    SliderVideoTime->setMinimum(0);


    SliderVideoTime->setTickInterval(1);
//    connect(SliderVideoTime, &QSlider::sliderMoved, this, &Window_Video_t::updateVideoTimePositionSliderMove);
    connect(Player, &QtAV::AVPlayer::positionChanged, this, &Window_Video_t::updateSilderTimeValue);
    connect(Player, &QtAV::AVPlayer::positionChanged, this, &Window_Video_t::positionVideoChanged);
    connect(Player, &QtAV::AVPlayer::stopped, Window_Control_Ptr, &Window_Control_t::videoStopEnd);
    connect(Player, &QtAV::AVPlayer::stopped, this, &Window_Video_t::videoStopEnd);

    connect(Player, &QtAV::AVPlayer::seekFinished, this, &Window_Video_t::seekFinished);

    Layout->addWidget(SliderVideoTime, 1, 0);

    LayoutVideoControl = new QGridLayout();

    Layout->addLayout(LayoutVideoControl, 2, 0);

    LabelVideoTime = new QLabel("00:00:00.00", this);
    LabelVideoTime->setFont(QFont("Times", 14));
    LabelVideoTime->setMaximumWidth(100);
    LayoutVideoControl->addWidget(LabelVideoTime, 0, 0);

    ButtonSeekBackward = new QPushButton("<<", this);
    ButtonSeekBackward->setMaximumWidth(50);
    connect(ButtonSeekBackward, &QPushButton::clicked, this, &Window_Video_t::seekBackward);
    LayoutVideoControl->addWidget(ButtonSeekBackward, 0, 1);

    ButtonPlay = new QPushButton("Play", this);
    connect(ButtonPlay, &QPushButton::clicked, this, &Window_Video_t::play);
    //connect(ButtonPlay, &QPushButton::clicked, Window_Editor_Ptr, &Window_Editor_t::videoPausePlayFromVideo);
    ButtonPlay->setEnabled(false);
    LayoutVideoControl->addWidget(ButtonPlay, 0, 2);

    ButtonSeekForward = new QPushButton(">>", this);
    ButtonSeekForward->setMaximumWidth(50);
    connect(ButtonSeekForward, &QPushButton::clicked, this, &Window_Video_t::seekForward);
    LayoutVideoControl->addWidget(ButtonSeekForward, 0, 3);

    ButtonFullScreen = new QPushButton("FullScreen", this);
    ButtonFullScreen->setMaximumWidth(100);
    connect(ButtonFullScreen, &QPushButton::clicked, this, &Window_Video_t::fullScreenVideo);
    ButtonFullScreen->setEnabled(false);
    LayoutVideoControl->addWidget(ButtonFullScreen, 0, 4);

    m_preview = new QtAV::VideoPreviewWidget(this);
    //m_preview->setWindowFlags(m_preview->windowFlags() |Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    m_preview->resize(200, 200);
    Layout->addWidget(m_preview, 0, 0);

}

void Window_Video_t::lol() {
    qDebug() << "looooooooool";
}

void Window_Video_t::hidePreviewAfterSeek() {
    m_preview->hide();
    disconnect(Player, &QtAV::AVPlayer::seekFinished, this, &Window_Video_t::hidePreviewAfterSeek);
}

void Window_Video_t::firstPlay(QString FileName) {
    m_FileName = FileName;
    ButtonPlay->setEnabled(true);
    Player->audio()->setVolume(0.0);
    connect(Player, &QtAV::AVPlayer::seekFinished, this, &Window_Video_t::hidePreviewAfterSeek);

    //Player->setAsyncLoad(false);

    //Player->setStartPosition(5000);
    Player->play(FileName);
}

uint32_t Window_Video_t::getPlayerPosition() {
    qDebug() << "Player state: " << Player->state();
    qDebug() << "Player media status: " << Player->mediaStatus();
    qDebug() << "Player in position: " << Player->position();
    return uint32_t(Player->position());
}

bool Window_Video_t::isPaused() {
    return !isPlaying;
}

uint32_t Window_Video_t::videoDuration() {
    return (uint32_t)Player->duration();
}

void Window_Video_t::seekFinished(qint64 pos) {
    signalSeekFinished(SavedPositionVideo);
    updateSilderTimeValue(Player->position());
    connect(Player, &QtAV::AVPlayer::positionChanged, this, &Window_Video_t::positionVideoChanged);
}

// public slots
void Window_Video_t::play() {
    ButtonPlay->setText("Pause");
    disconnect(ButtonPlay, &QPushButton::clicked, this, &Window_Video_t::play);
    connect(ButtonPlay,&QPushButton::clicked, this, &Window_Video_t::pause);
    signalVideoPlay();
    Player->pause(false);
    isPlayingSliderPress = true;
    isPlaying = true;
    //Window_Editor_Ptr->videoPausePlayFromVideo(false);
    m_preview->hide();
}

void Window_Video_t::pause() {
    ButtonPlay->setText("Play");
    disconnect(ButtonPlay, &QPushButton::clicked, this, &Window_Video_t::pause);
    connect(ButtonPlay,&QPushButton::clicked, this, &Window_Video_t::play);
    signalVideoPause();
    Player->pause(true);
    isPlayingSliderPress = false;
    isPlaying = false;
    //Window_Editor_Ptr->videoPausePlayFromVideo(true);
}

void Window_Video_t::updateVideoPositionEditorSlider(uint32_t pos) {
    //qDebug() << "time update slider";
    disconnect(Player, &QtAV::AVPlayer::positionChanged, this, &Window_Video_t::positionVideoChanged);
    sliderEditorSeek = true;
    SavedPositionVideo = pos * 100;
    Player->setPosition(qint64(SavedPositionVideo));

}


// private slots
void Window_Video_t::playInit() {
    Player->pause(true);

    m_preview->setFile(m_FileName);
    m_preview->setTimestamp(100);
    m_preview->preview();
    m_preview->show();

    SliderVideoTime->setMaximum(Player->duration());
    Window_Editor_Ptr->setAfterVideoLoad(Player->duration());
}

//void Window_Video_t::updateVideoTimePositionSliderMove(int newPlayerTimePosition) {
//    Player->seek(qint64(newPlayerTimePosition));
//}

//void Window_Video_t::updateVideoTimePositionSliderPressed() {
//    Player->seek(qint64(SliderVideoTime->value()));
//}

void Window_Video_t::updateSilderTimeValue(qint64 newSliderPosition) {
    //positionVideoChanged(newSliderPosition);
    qDebug() << "222  Player->position(): " << Player->position();
    SliderVideoTime->setValue(newSliderPosition);
    LabelVideoTime->setText(miliSecToTime(newSliderPosition));
}

void Window_Video_t::seekBackward() {
    Player->seek(Player->position() - 5000);
}

void Window_Video_t::seekForward() {
    Player->seek(Player->position() + 5000);
}

void Window_Video_t::fullScreenVideo() {
    if (IsFullScreen) {
        this->showNormal();
    }
    else {
        qDebug() << "full";
        this->showFullScreen();
    }
    IsFullScreen = !IsFullScreen;
}


// moc nefunguje
void Window_Video_t::videoStopEnd(){
    ButtonPlay->setText("Play");
    disconnect(ButtonPlay, &QPushButton::clicked, this, &Window_Video_t::pause);
    connect(ButtonPlay, &QPushButton::clicked, this, &Window_Video_t::play);
    disconnect(ButtonPlay, &QPushButton::clicked, this, &Window_Video_t::signalVideoPause);
    connect(ButtonPlay, &QPushButton::clicked, this, &Window_Video_t::signalVideoPlay);
    isPlaying = false;
    Player->play(m_FileName);
}

