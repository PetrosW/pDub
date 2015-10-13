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

class Window_Control_t : public QMainWindow
{
    Q_OBJECT

    public:
        Window_Control_t(QMainWindow *parent = 0);
        void setWindowEditorPtr(Window_Editor_t *Window_Editor);
        void setWindowVideoPtr(Window_Video_t *Window_Video);
        void setDeafaultMicrophone();

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

    private slots:
        void newProject();
        void loadProject();
        void saveProject();
        void exportProject();

};

#endif
