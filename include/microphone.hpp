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
        void createUi();

        Window_Video_t *Window_Video_Ptr;
        Window_Control_t *Window_Control_Ptr;
        QAudioRecorder *AudioRecorder;
        QTimer *TimerRecord;

        QGridLayout *Layout;
        QLabel *LabelStartTime;
        QLabel *LabelEndTime;
        QLabel *LabelDurationTime;
        QPushButton *ButtonRecord;
        QPushButton *ButtonPause;

        int DurationTime;
        int StartTime;
        int EndTime;

    private slots:
        void startRecord();
        void stopRecord();
        void timerRecordTick();

    signals:
        void recordingEnd(int RecordId, int StartTime, int EndTime, QString Name);
};

#endif // MICROPHONE

