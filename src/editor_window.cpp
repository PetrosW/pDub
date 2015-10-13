#include <windows/editor_window.hpp>

Window_Editor_t::Window_Editor_t(Window_Control_t *Window_Control, QWidget *Window_Control_QWidget)
    : QWidget(Window_Control_QWidget),
      Window_Control_Ptr(Window_Control),
      Window_Video_Ptr(nullptr)
{
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint);
    setWindowTitle("Editor");


    show();
}

void Window_Editor_t::setWindowVideoPtr(Window_Video_t *Window_Video)
{
    Window_Video_Ptr = Window_Video;
}

//public slots

void Window_Editor_t::addNewRecordObject(int StartTime, int EndTime, QString Name) {
    qDebug() << StartTime;
    qDebug() << EndTime;
    qDebug() << Name;
}
