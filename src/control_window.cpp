#include <windows/control_window.hpp>

Window_Control_t::Window_Control_t() : Window_Editor_Ptr(nullptr), Window_Video_Ptr(nullptr)
{
    setWindowTitle("pDub");
    show();
}

void Window_Control_t::setWindowEditorPtr(Window_Editor_t *Window_Editor)
{
    Window_Editor_Ptr = Window_Editor;
}

void Window_Control_t::setWindowVideoPtr(Window_Video_t *Window_Video)
{
    Window_Video_Ptr = Window_Video;
}
