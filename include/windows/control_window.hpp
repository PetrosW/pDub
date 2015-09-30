#ifndef CONTROL_WINDOW_HPP
#define CONTROL_WINDOW_HPP

#include <QEvent>
#include <QMainWindow>

class Window_Control_t : public QMainWindow
{
    Q_OBJECT

    public:
        Window_Control_t();
        void setWindowEditorPtr(QWidget *Window_Editor);
        void setWindowVideoPtr(QWidget *Window_Video);

    private:
        QWidget *Window_Editor_Ptr;
        QWidget *Window_Video_Ptr;
};

#endif
