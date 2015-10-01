#ifndef EDITOR_WINDOW_HPP
#define EDITOR_WINDOW_HPP

#include <QWidget>

#include <common.hpp>
#include <windows/control_window.hpp>
#include <windows/video_window.hpp>

class Window_Editor_t : public QWidget
{
    Q_OBJECT

    public:
        Window_Editor_t(Window_Control_t *Window_Control, QWidget *Window_Control_QWidget);
        void setWindowVideoPtr(Window_Video_t *Window_Video);

    private:
        Window_Control_t *Window_Control_Ptr;
        Window_Video_t *Window_Video_Ptr;
};

#endif
