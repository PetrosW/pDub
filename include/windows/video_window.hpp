#ifndef VIDEO_WINDOW_HPP
#define VIDEO_WINDOW_HPP

#include <QWidget>

#include <common.hpp>
#include <windows/control_window.hpp>
#include <windows/editor_window.hpp>

class Window_Video_t : public QWidget
{
    Q_OBJECT

    public:
        Window_Video_t(Window_Control_t *Window_Control, QWidget *Window_Control_QWidget);
        void setWindowEditorPtr(Window_Editor_t *Window_Editor);

    private:
        Window_Control_t *Window_Control_Ptr;
        Window_Editor_t *Window_Editor_Ptr;
};

#endif
