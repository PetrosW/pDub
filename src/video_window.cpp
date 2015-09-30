#include <windows/video_window.hpp>

Window_Video_t::Window_Video_t(QWidget *Window_Control) : QWidget(Window_Control), Window_Editor_Ptr(nullptr)
{
    setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint);
    setWindowTitle("Video");
    show();
}

void Window_Video_t::setWindowEditorPtr(QWidget *Window_Editor)
{
    Window_Editor_Ptr = Window_Editor;
}
