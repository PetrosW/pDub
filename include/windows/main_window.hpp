#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <windows/control_window.hpp>
#include <windows/video_window.hpp>
#include <windows/editor_window.hpp>

class Window_Main_t : public QMainWindow
{
    Q_OBJECT

    public:
        Window_Main_t(QWidget *parent = nullptr);
        void closeEvent(QCloseEvent *event);

    private:
        QVBoxLayout *Layout_Vertical;
        QHBoxLayout *Layout_Horizontal;
        bool IsDocked = true;
        Window_Control_t *Window_Control_Ptr;
        Window_Video_t *Window_Video_Ptr;
        Window_Editor_t *Window_Editor_Ptr;

        void createToolBar();

        QAction *A_newProject;
        QAction *A_loadProject;
        QAction *A_saveProject;
        QAction *A_exportProject;
        QAction *A_importAudio;

    signals:

    public slots:
        void dockingChange();

    private slots:
        void newProject(QString projectName, QString videoFilePath, QString projectFolder);
        void newProjectDialog();
        void loadProject();
        void saveProject();
        void exportProject();
        void importAudio();
};

#endif // MAIN_WINDOW_H
