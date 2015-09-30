#ifndef EDITOR_WINDOW_HPP
#define EDITOR_WINDOW_HPP

#include <QWidget>

class Window_Editor_t : public QWidget
{
    Q_OBJECT

    public:
        Window_Editor_t(QWidget *Window_Control);
        void setWindowVideoPtr(QWidget *Window_Video);

    private:
        // K Window_Control se da dostat pres parent()
        QWidget *Window_Video_Ptr;
};

#endif
