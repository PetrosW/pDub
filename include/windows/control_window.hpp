#ifndef CONTROL_WINDOW_HPP
#define CONTROL_WINDOW_HPP

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QKeySequence>

#include <common.hpp>
#include <microphone.hpp>
#include <windows/editor_window.hpp>
#include <windows/video_window.hpp>
#include <windows/main_window.hpp>
#include <dialogs/newproject_dialog.hpp>
#include <ffmpeg/ffmpeg.hpp>

class Window_Control_t : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QString ProjectFolder READ ProjectFolder WRITE setProjectFolder NOTIFY ProjectFolderChanged)
    Q_PROPERTY(QString RecordPath READ RecordPath WRITE setRecordPath NOTIFY RecordPathChanged)
    Q_PROPERTY(QString ProjectName READ ProjectName WRITE setProjectName NOTIFY ProjectNameChanged)
    Q_PROPERTY(QString VideoFilePath READ VideoFilePath WRITE setVideoFilePath NOTIFY VideoFilePathChanged)

    public:
        Window_Control_t(QWidget *parent = nullptr);
        void setWindowEditorPtr(Window_Editor_t *Window_Editor);
        void setWindowVideoPtr(Window_Video_t *Window_Video);
        void setDeafaultMicrophone();
        QString getProjectFolder();

        int NextRecordId;

        QString ProjectFolder() const { return m_ProjectFolder; }
        QString RecordPath() const { return m_RecordPath; }
        QString ProjectName() const { return m_ProjectName; }
        QString VideoFilePath() const { return m_VideoFilePath; }

        void setProjectFolder(const QString &a) { m_ProjectFolder = a; ProjectFolderChanged(); }
        void setRecordPath(const QString &a) { m_RecordPath = a; RecordPathChanged(); }
        void setProjectName(const QString &a) { m_ProjectName = a; ProjectNameChanged(); }
        void setVideoFilePath(const QString &a) { m_VideoFilePath = a; VideoFilePathChanged(); }

        QPushButton *ButtonDockWindowVideo;

        void createUi();

    private:
        Window_Editor_t *Window_Editor_Ptr;
        Window_Video_t *Window_Video_Ptr;



        QWidget *MainWidget;
        QGridLayout *Layout;
        QGridLayout *ControlLayout;

        void newMicrophone();


        QString m_ProjectName;
        QString m_VideoFilePath;
        QString m_ProjectFolder;
        QString TmpPath;
        QString m_RecordPath;



    signals:
        void ProjectFolderChanged();
        void RecordPathChanged();
        void ProjectNameChanged();
        void VideoFilePathChanged();
        void VideoStop();

    public slots:
        void videoStopEnd();

    private slots:


};

#endif
