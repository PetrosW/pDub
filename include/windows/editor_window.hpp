#ifndef EDITOR_WINDOW_HPP
#define EDITOR_WINDOW_HPP

#include <QWidget>
#include <QMap>

#include <common.hpp>
#include <record.hpp>
#include <windows/control_window.hpp>
#include <windows/video_window.hpp>

class Window_Editor_t : public QWidget
{
    Q_OBJECT

    public:
        Window_Editor_t(Window_Control_t *Window_Control, QWidget *Window_Control_QWidget);
        void setWindowVideoPtr(Window_Video_t *Window_Video);
        void setAfterVideoLoad(qint64 duration);

    private:

        Window_Control_t *Window_Control_Ptr;
        Window_Video_t *Window_Video_Ptr;

        QGridLayout *Layout;
        QGridLayout *ControlLayout;

        QWidget *SliderWidget;
        QWidget *WorkPlaceWidget;

        QScrollArea *WorkPlaceScrollArea;

        QPushButton *ButtonDelete;
        QPushButton *ButtonSplit;
        QPushButton *ButtonMerge;
        QPushButton *ButtonForward;
        QPushButton *ButtonBackward;

        QLabel *LabelVideoTime;
        QLabel *LabelRecordStartTime;
        QLabel *LabelRecordEndTime;

        QMap<int, Record *> MapRecord;

        void createUi();

    public slots:

        void addNewRecordObject(int RecordID, int StartTime, int EndTime, QString Name);
};

#endif
