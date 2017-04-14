#include <windows/video_window.hpp>

Window_Video_t::Window_Video_t(QWidget *parent) : QWidget(parent), Window_Control_Ptr(nullptr), Window_Editor_Ptr(nullptr)
{
    QtAV::Widgets::registerRenderers();

    Layout = new QGridLayout();
    Layout->setMargin(0);
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

    connect(Player, &QtAV::AVPlayer::positionChanged, this, &Window_Video_t::positionVideoChanged);
    connect(Player, &QtAV::AVPlayer::stopped, Window_Control_Ptr, &Window_Control_t::videoStopEnd);
    connect(Player, &QtAV::AVPlayer::stopped, this, &Window_Video_t::videoStopEnd);

    connect(Player, &QtAV::AVPlayer::seekFinished, this, &Window_Video_t::seekFinished);

    shortcutFullScreen = new QShortcut(QKeySequence("f"), this);
    connect(shortcutFullScreen, &QShortcut::activated, Window_Control_Ptr, &Window_Control_t::fullScreenVideo);

    m_preview = new QtAV::VideoPreviewWidget(this);
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
    Player->audio()->setVolume(0.0);
    connect(Player, &QtAV::AVPlayer::seekFinished, this, &Window_Video_t::hidePreviewAfterSeek);
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
    connect(Player, &QtAV::AVPlayer::positionChanged, this, &Window_Video_t::positionVideoChanged);
}

// public slots
void Window_Video_t::play() {
    signalVideoPlay();
    Player->pause(false);
    isPlaying = true;
    m_preview->hide();
}

void Window_Video_t::pause() {
    signalVideoPause();
    Player->pause(true);
    isPlaying = false;
}

void Window_Video_t::updateVideoPositionEditorSlider(uint32_t pos) {
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

    Window_Editor_Ptr->setAfterVideoLoad(Player->duration());
}


void Window_Video_t::updateLabelVideoTime(qint64 newPosition) {
    qDebug() << "222  Player->position(): " << Player->position();
}

void Window_Video_t::seekBackward() {
    Player->seek(Player->position() - 5000);
}

void Window_Video_t::seekForward() {
    Player->seek(Player->position() + 5000);
}


// moc nefunguje
void Window_Video_t::videoStopEnd(){
    ButtonPlay->setText("Play");
    isPlaying = false;
    Player->play(m_FileName);
}

void Window_Video_t::m_repaint(){
    qDebug() << "update";
    resize(640,360);
    updateGeometry();
    VideoOutput->widget()->updateGeometry();
}

