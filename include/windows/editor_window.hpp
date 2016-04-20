#ifndef EDITOR_WINDOW_HPP
#define EDITOR_WINDOW_HPP


#include <record.hpp>
#include <QWidget>
#include <QMap>
#include <QtAV>
#include <QMediaPlayer>
#include <QVector>
#include <QTimer>
//#include <QCheckBox>

#include <common.hpp>

#include <slider_editor.hpp>
#include <windows/control_window.hpp>
#include <windows/video_window.hpp>
#include <record_workplace.hpp>


class Window_Editor_t : public QWidget
{
    Q_OBJECT

    public:
        Window_Editor_t(Window_Control_t *Window_Control, QWidget *Window_Control_QWidget);

        QMap<uint32_t, QMap<uint32_t, Record *> > MapTimeRecord;
        QMap<uint32_t, Record *> MapRecord;

        Ffmpeg_t *m_ffmpeg;

        void setWindowVideoPtr(Window_Video_t *Window_Video);
        void setAfterVideoLoad(qint64 duration);
        void createUi();



    private:

        Window_Control_t *Window_Control_Ptr;
        Window_Video_t *Window_Video_Ptr;

        QVector<QtAV::AVPlayer *> VectorMediaPlayer;

        QTimer *TimerNextPlayRecord;
        uint32_t NextPlayingStartTime;
        uint32_t NextPlayintId;

        uint32_t SelectedRecordId;

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

        QCheckBox *CheckBoxMuteRecords;

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
        void updateRecordPlayer();
        void updateRecordPlayerTimer();
        void relocateRecordInMap(uint32_t RecordID, uint32_t OldStartTime);
        void split();

    public slots:
        void setSliderLinePosition(uint32_t pos);
        void videoPausePlayFromVideo(bool isPause);
        void addNewRecordObject(uint32_t RecordId, uint32_t StartTime, uint32_t EndTime, QString Name);
        void recordSelected(uint32_t RecordId, uint32_t StartTime, uint32_t EndTime, QString Name);
        void recordMoveSelected(uint32_t RecordId, uint32_t StartTime, uint32_t EndTime, QString Name);
};

#endif
