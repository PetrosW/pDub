#include <windows/control_window.hpp>

Window_Control_t::Window_Control_t(QMainWindow *parent) : QMainWindow(parent), Window_Editor_Ptr(nullptr), Window_Video_Ptr(nullptr)
{

    createUi();
    createToolBar();
    newMicrophone();
    show();
    //nahození za jízdy, čili za show funguje
    /*QPushButton *button = new QPushButton("Hello");
    layout->addWidget(button);*/
}

void Window_Control_t::setWindowEditorPtr(Window_Editor_t *Window_Editor)
{
    Window_Editor_Ptr = Window_Editor;
}

void Window_Control_t::setWindowVideoPtr(Window_Video_t *Window_Video)
{
    Window_Video_Ptr = Window_Video;
}

void Window_Control_t::newMicrophone() {
    Microphone *mic1 = new Microphone(this);
    Layout->addWidget(mic1,0,1);
    Layout->setColumnMinimumWidth(1, 100);
}

void Window_Control_t::createUi() {
    setWindowTitle("pDub");
    MainWidget = new QWidget(this);
    this->setCentralWidget(MainWidget);
    Layout = new QGridLayout(this);
    Layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    Layout->setColumnMinimumWidth(0, 100);
    MainWidget->setLayout(Layout);


    QGridLayout *ControlLayout = new QGridLayout();
    Layout->addLayout(ControlLayout, 0, 0);

    QPushButton *test = new QPushButton("test");
    ControlLayout->addWidget(test, 0, 0);



}

void Window_Control_t::createToolBar() {

    QMenuBar *MenuBar = new QMenuBar(this);
    this->setMenuBar(MenuBar);


//    newAct = new QAction(tr("&New"), this);
//    newAct->setShortcuts(QKeySequence::New);
//    newAct->setStatusTip(tr("Create a new file"));
//    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));


    A_newProject = new QAction(tr("&New"), this);
    A_newProject->setShortcuts(QKeySequence::New);
    A_newProject->setStatusTip(tr("Create a new project"));
    connect(A_newProject, SIGNAL(triggered()), this, SLOT(newProject()));

    A_loadProject = new QAction(tr("&Open"), this);
    A_loadProject->setShortcuts(QKeySequence::Open);
    A_loadProject->setStatusTip(tr("O a old Project"));
    connect(A_loadProject, SIGNAL(triggered()), this, SLOT(loadProject()));

    A_saveProject = new QAction(tr("&Save"), this);
    A_saveProject->setShortcuts(QKeySequence::Save);
    A_saveProject->setStatusTip(tr("Save a project"));
    connect(A_saveProject, SIGNAL(triggered()), this, SLOT(saveProject()));

    A_exportProject = new QAction(tr("&Export"), this);
    A_exportProject->setShortcut(tr("Ctrl+E"));
    A_exportProject->setStatusTip(tr("Export a project"));
    connect(A_exportProject, SIGNAL(triggered()), this, SLOT(exportProject()));

    MenuBar->addAction(A_newProject);
    MenuBar->addAction(A_loadProject);
    MenuBar->addAction(A_saveProject);
    MenuBar->addAction(A_exportProject);

    //toolbar: dá se všemožně posouvat atd
    /*QToolBar *ToolBar = new QToolBar(this);
    this->addToolBar(ToolBar);
    ToolBar->addAction("New project");*/
}

//private slots

void Window_Control_t::newProject() {
    qDebug() << "newnew";
}
void Window_Control_t::loadProject() {
    qDebug() << "load";
}
void Window_Control_t::saveProject() {
    qDebug() << "save";
}
void Window_Control_t::exportProject() {
    qDebug() << "export";
}

