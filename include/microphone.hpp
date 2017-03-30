#ifndef MICROPHONE_HPP
#define MICROPHONE_HPP

#include <common.hpp>
#include <windows/video_window.hpp>

#include <QWidget>
#include <QtWidgets>
#include <QDir>
#include <QUrl>
#include <QAudioRecorder>


class Microphone : public QWidget
{
    Q_OBJECT

    public:
        Microphone(Window_Control_t *Window_Control_ptr, Window_Video_t *Window_Video_ptr, QWidget *parent = 0);
        ~Microphone();



    private:
        Window_Control_t *Window_Control_Ptr;
        Window_Video_t *Window_Video_Ptr;
        QAudioRecorder *AudioRecorder;
        QTimer *TimerRecord;

        QGridLayout *Layout;
        QLabel *LabelStartTime;
        QLabel *LabelEndTime;
        QLabel *LabelDurationTime;
        QPushButton *ButtonRecord;
        QPushButton *ButtonPause;

        uint32_t DurationTime;
        uint32_t StartTime;
        uint32_t EndTime;

        void createUi();

    public slots:
        void videoStopEnd();

    private slots:
        void startRecord();
        void stopRecord();
        void timerRecordTick();

    signals:
        void recordingEnd(uint32_t RecordId, uint32_t StartTime, uint32_t EndTime, QString Name, uint32_t RowPosition, uint32_t Volume);
};

#endif // MICROPHONE

