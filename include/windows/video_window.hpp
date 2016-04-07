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
        Window_Video_t(Window_Control_t *Window_Control, QWidget *Window_Control_QWidget);
        void setWindowEditorPtr(Window_Editor_t *Window_Editor);

        void firstPlay(QString FileName);
        int getPlayerPosition();
        bool isPaused();
        bool sliderEditorSeek;

        void createUi();

    private:
        Window_Control_t *Window_Control_Ptr;
        Window_Editor_t *Window_Editor_Ptr;

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

        bool isPlayingSliderPress; // pomocna pro urcovani pausi pred kliknutim na slider
        bool isPlaying;

    signals:
        void positionVideoChanged(qint64 pos);
        void signalVideoTimePositionSliderMove();

    public slots:
        void play();
        void pause();
        void updateVideoPositionEditorSlider(uint32_t pos);


    private slots:
        void playInit();
        void seekForward();
        void seekBackward();
        void setVolume(int newVolume);
        void updateSilderTimeValue(qint64 newSliderPosition);
        void updateVideoTimePositionSliderMove(int newPlayerTimePosition);
        void updateVideoTimePositionSliderPressed();
        void sliderPressRelease();
};

#endif
