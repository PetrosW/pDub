#ifndef EDITOR_WINDOW_HPP
#define EDITOR_WINDOW_HPP

#include <QWidget>
#include <QMap>

#include <common.hpp>
#include <record.hpp>
#include <slider_editor.hpp>
#include <windows/control_window.hpp>
#include <windows/video_window.hpp>
#include <record_workplace.hpp>

class Window_Editor_t : public QWidget
{
    Q_OBJECT

    public:
        Window_Editor_t(Window_Control_t *Window_Control, QWidget *Window_Control_QWidget);

        QMap<int, Record *> MapRecord;

        void setWindowVideoPtr(Window_Video_t *Window_Video);
        void setAfterVideoLoad(qint64 duration);
        void createUi();

    private:

        Window_Control_t *Window_Control_Ptr;
        Window_Video_t *Window_Video_Ptr;

        QGridLayout *Layout;
        QGridLayout *ControlLayout;

        SliderEditor *SliderEditorControl;

        QWidget *WidgetSlider;
        QWidget *WidgetWorkPlace;
        RecordWorkplace *WidgetRecordWorkPlace;

        QWidget *SliderLine;

        QScrollArea *ScrollAreaEditorTimeSlider;
        QScrollBar *ScrollBarEditorTimeSliderHorizontal;

        QScrollArea *ScrollAreaWorkPlace;
        QScrollBar *ScrollBarWorkPlaceVertical;
        QScrollBar *ScrollBarWorkPlaceHorizontal;

        QPushButton *ButtonDelete;
        QPushButton *ButtonSplit;
        QPushButton *ButtonMerge;
        QPushButton *ButtonForward;
        QPushButton *ButtonBackward;

        QPushButton *ButtonAddRow;

        QLabel *LabelVideoTime;
        QLabel *LabelRecordStartTime;
        QLabel *LabelRecordEndTime;



    protected:
        void paintEvent(QPaintEvent *);

    signals:
        void sliderLinePositionChanged(uint32_t pos);

    private slots:
        void addRow();
        void setScrollAreaEditorTimeSliderValue(int pos);
        void setSliderLinePositionFromVideo(qint64 pos);

    public slots:
        void setSliderLinePosition(uint32_t pos);
        void addNewRecordObject(int RecordID, int StartTime, int EndTime, QString Name);
};

#endif
