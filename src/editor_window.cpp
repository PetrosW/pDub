#include <windows/editor_window.hpp>

Window_Editor_t::Window_Editor_t(QWidget *Window_Control) : QWidget(Window_Control), Window_Video_Ptr(nullptr)
{
    setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint);
    setWindowTitle("Editor");
    show();
}

void Window_Editor_t::setWindowVideoPtr(QWidget *Window_Video)
{
    Window_Video_Ptr = Window_Video;
}
