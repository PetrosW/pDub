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
#include <dialogs/newproject_dialog.hpp>

class Window_Control_t : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(QString ProjectFolder READ ProjectFolder WRITE setProjectFolder NOTIFY ProjectFolderChanged)
    Q_PROPERTY(QString RecordPath READ RecordPath WRITE setRecordPath NOTIFY RecordPathChanged)

    public:
        Window_Control_t(QMainWindow *parent = 0);
        void setWindowEditorPtr(Window_Editor_t *Window_Editor);
        void setWindowVideoPtr(Window_Video_t *Window_Video);
        void setDeafaultMicrophone();
        QString getProjectFolder();

        int NextRecordId;

        QString ProjectFolder() const { return m_ProjectFolder; }
        QString RecordPath() const { return m_RecordPath; }


    private:
        Window_Editor_t *Window_Editor_Ptr;
        Window_Video_t *Window_Video_Ptr;

        QWidget *MainWidget;
        QGridLayout *Layout;
        QGridLayout *ControlLayout;


        void newMicrophone();
        void createUi();
        void createToolBar();

        QAction *A_newProject;
        QAction *A_loadProject;
        QAction *A_saveProject;
        QAction *A_exportProject;

        QString ProjectName;
        QString VideoFilePath;
        QString m_ProjectFolder;
        QString TmpPath;
        QString m_RecordPath;

        void setProjectFolder(const QString &a) { m_ProjectFolder = a; ProjectFolderChanged(); }
        void setRecordPath(const QString &a) { m_RecordPath = a; RecordPathChanged(); }

    signals:
        void ProjectFolderChanged();
        void RecordPathChanged();

    private slots:
        void newProject(QString projectName, QString videoFilePath, QString projectFolder);
        void newProjectDialog();
        void loadProject();
        void saveProject();
        void exportProject();

};

#endif
