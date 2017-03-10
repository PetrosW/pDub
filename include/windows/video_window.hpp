#ifndef VIDEO_WINDOW_HPP
#define VIDEO_WINDOW_HPP

#include <QWidget>
#include <QtWidgets>
#include <QtAV>
#include <QtAVWidgets>

#include <common.hpp>
#include <windows/control_window.hpp>
#include <windows/editor_window.hpp>

class Window_Video_t : public QWidget
{
    Q_OBJECT

    public:
        Window_Video_t(QWidget *parent = nullptr);
        void setWindowEditorPtr(Window_Editor_t *Window_Editor);
        void setWindowControlPtr(Window_Control_t *Window_Control);

        void firstPlay(QString FileName);
        int getPlayerPosition();
        bool isPaused();
        bool sliderEditorSeek;
        uint32_t videoDuration();

        void createUi();

    private:
        QString m_FileName;

        Window_Control_t *Window_Control_Ptr;
        Window_Editor_t *Window_Editor_Ptr;

        QtAV::VideoPreviewWidget *m_preview;

        QGridLayout *Layout;
        QGridLayout *LayoutVideoControl;
        QLabel *LabelVideoTime;
        QtAV::VideoOutput *VideoOutput;
        QtAV::AVPlayer *Player;

        QPushButton *ButtonPlay;
        QPushButton *ButtonSeekForward;
        QPushButton *ButtonSeekBackward;
        QSlider *SliderVideoTime;
        QSlider *SliderVideoVolume;

        QPushButton *ButtonDockWindowVideo;

        bool isPlayingSliderPress; // pomocna pro urcovani pauzi pred kliknutim na slider
        bool isPlaying;

    signals:
        void positionVideoChanged(qint64 pos);
        void signalVideoTimePositionSliderMove();
        void signalVideoPlay();
        void signalVideoPause();

    public slots:
        void play();
        void pause();
        void updateVideoPositionEditorSlider(uint32_t pos);
        void setMute(bool set);


    private slots:
        void playInit();
        void seekForward();
        void seekBackward();
        void setVolume(int newVolume);
        void updateSilderTimeValue(qint64 newSliderPosition);
        void updateVideoTimePositionSliderMove(int newPlayerTimePosition);
        void updateVideoTimePositionSliderPressed();
        void sliderPressRelease();
        void videoStopEnd();
};

#endif
