#ifndef NEWPROJECT_DIALOG_H
#define NEWPROJECT_DIALOG_H

#include <common.hpp>
#include <QWidget>
#include <QtWidgets>
#include <QFileInfo>
#include <QMessageBox>

class newProject_dialog : public QDialog
{
    Q_OBJECT
public:
    explicit newProject_dialog(QWidget *parent = 0);

private:
    QPushButton *ButtonSelectVideo;
    QPushButton *ButtonSelectProjectFolder;
    QLabel *LabelProjectName;
    QLabel *LabelSelectVideo;
    QLabel *LabelSelectProjectFolder;
    QLineEdit *LineEditProjectName;
    QLineEdit *LineEditSelectVideo;
    QLineEdit *LineEditSelectProjectFolder;

    QPushButton *ButtonOk;
    QPushButton *ButtonCancel;

signals:
    void accepted(QString projectName, QString videoFilePath, QString projectFolder);
public slots:
private slots:
    void accept();
    void selectVideo();
    void selectFolder();
};

#endif // NEWPROJECT_DIALOG_H
