#ifndef MICROPHONE_HPP
#define MICROPHONE_HPP

#include <common.hpp>
#include <QWidget>
#include <QtWidgets>
#include <QDir>
#include <QUrl>
#include <QAudioRecorder>


class Microphone : public QWidget
{
    Q_OBJECT

    public:
        Microphone(QWidget *parent = 0);
        ~Microphone();
    private:
        void createUi();

        QAudioRecorder *AudioRecorder;

        QGridLayout *Layout;
        QPushButton *ButtonRecord;
        QPushButton *ButtonPause;

    private slots:
        void startRecord();
        void stopRecord();
};

#endif // MICROPHONE

