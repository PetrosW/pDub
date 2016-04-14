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

    connect(Player, &QtAV::AVPlayer::started, this, &Window_Video_t::playInit);
    //connect(Player, SIGNAL(seekFinished(qint64)), this, SLOT(onSeekFinished(qint64)));
    Layout->addWidget(VideoOutput->widget(), 0, 0);

    sliderEditorSeek = false;
    isPlayingSliderPress = false;
    isPlaying = false;

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
    connect(SliderVideoTime, &QSlider::sliderMoved, this, &Window_Video_t::updateVideoTimePositionSliderMove);
    connect(SliderVideoTime, &QSlider::sliderReleased, this, &Window_Video_t::signalVideoTimePositionSliderMove);
    connect(SliderVideoTime, &QSlider::sliderReleased, this, &Window_Video_t::sliderPressRelease);
    connect(SliderVideoTime, &QSlider::sliderPressed, this, &Window_Video_t::sliderPressRelease);
    connect(Player, &QtAV::AVPlayer::positionChanged, this, &Window_Video_t::updateSilderTimeValue);
    connect(Player, &QtAV::AVPlayer::positionChanged, this, &Window_Video_t::positionVideoChanged);


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
    //connect(ButtonPlay, &QPushButton::clicked, Window_Editor_Ptr, &Window_Editor_t::videoPausePlayFromVideo);
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

bool Window_Video_t::isPaused() {
    return !isPlaying;
}

uint32_t Window_Video_t::videoDuration() {
    return (uint32_t)Player->duration();
}

// public slots
void Window_Video_t::play() {
    ButtonPlay->setText("Pause");
    disconnect(ButtonPlay, SIGNAL(clicked()), this, SLOT(play()));
    connect(ButtonPlay, SIGNAL(clicked()), this, SLOT(pause()));
    Player->pause(false);
    isPlayingSliderPress = true;
    isPlaying = true;
    Window_Editor_Ptr->videoPausePlayFromVideo(false);
}

void Window_Video_t::pause() {
    ButtonPlay->setText("Play");
    disconnect(ButtonPlay, SIGNAL(clicked()), this, SLOT(pause()));
    connect(ButtonPlay, SIGNAL(clicked()), this, SLOT(play()));
    Player->pause(true);
    isPlayingSliderPress = false;
    isPlaying = false;
    Window_Editor_Ptr->videoPausePlayFromVideo(true);
}

void Window_Video_t::updateVideoPositionEditorSlider(uint32_t pos) {
    //qDebug() << "time update slider";
    disconnect(Player, &QtAV::AVPlayer::positionChanged, this, &Window_Video_t::positionVideoChanged);
    sliderEditorSeek = true;
    Player->seek(qint64(pos * 100));
    connect(Player, &QtAV::AVPlayer::positionChanged, this, &Window_Video_t::positionVideoChanged);
}

// private slots
void Window_Video_t::playInit() {
    SliderVideoTime->setMaximum(Player->duration());
    Window_Editor_Ptr->setAfterVideoLoad(Player->duration());
    //Player->setBufferMode(QtAV::BufferTime);
    //120000000 = 120 MB
    //Player->setBufferValue(60000);
    Player->setSeekType(QtAV::AccurateSeek);
    while (!Player->isPlaying() || !Player->isPaused()) {
        if (Player->isPlaying()) {
            Player->setPosition(qint64(0));
            Player->pause(true);
            Player->setSeekType(QtAV::AccurateSeek);
        }
    }
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
    //positionVideoChanged(newSliderPosition);
    SliderVideoTime->setValue(newSliderPosition);
    LabelVideoTime->setText(miliSecToTime(newSliderPosition));
    if (sliderEditorSeek == false) {
        positionVideoChanged(newSliderPosition);
    }
    sliderEditorSeek = false;
}

void Window_Video_t::seekBackward() {
    Player->seek(Player->position() - 5000);
}

void Window_Video_t::seekForward() {
    Player->seek(Player->position() + 5000);
}

void Window_Video_t::sliderPressRelease() {
    if (isPlayingSliderPress == true) {
        bool check = Player->isPaused();
        qDebug() << Player->isPaused();
        Player->pause(!Player->isPaused());
        qDebug() << Player->isPaused();
        while(check == Player->isPaused()) {
            qDebug() << "checkuju";
        }
        isPlaying = Player->isPaused();
        Window_Editor_Ptr->videoPausePlayFromVideo(isPlaying);
    }
}
