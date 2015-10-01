#ifndef CONTROL_WINDOW_HPP
#define CONTROL_WINDOW_HPP

#include <QMainWindow>

#include <common.hpp>
#include <windows/editor_window.hpp>
#include <windows/video_window.hpp>

class Window_Control_t : public QMainWindow
{
    Q_OBJECT

    public:
        Window_Control_t();
        void setWindowEditorPtr(Window_Editor_t *Window_Editor);
        void setWindowVideoPtr(Window_Video_t *Window_Video);

    private:
        Window_Editor_t *Window_Editor_Ptr;
        Window_Video_t *Window_Video_Ptr;
};

#endif
