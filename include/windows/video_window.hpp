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

    private:
        Window_Control_t *Window_Control_Ptr;
        Window_Editor_t *Window_Editor_Ptr;

        QGridLayout *Layout;
        QtAV::VideoOutput *VideoOutput;
        QtAV::AVPlayer *Player;

        QPushButton *ButtonPlay;

        void createUi();

    private slots:
        void play();
        void pause();
};

#endif
