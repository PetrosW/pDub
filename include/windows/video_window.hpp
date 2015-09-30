#ifndef VIDEO_WINDOW_HPP
#define VIDEO_WINDOW_HPP

#include <QWidget>

class Window_Video_t : public QWidget
{
    Q_OBJECT

    public:
        Window_Video_t(QWidget *Window_Control);
        void setWindowEditorPtr(QWidget *Window_Editor);

    private:
        // K Window_Control se da dostat pres parent()
        QWidget *Window_Editor_Ptr;
};

#endif
