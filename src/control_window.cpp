#include <windows/control_window.hpp>

Window_Control_t::Window_Control_t(QMainWindow *parent) : QMainWindow(parent), Window_Editor_Ptr(nullptr), Window_Video_Ptr(nullptr)
{

    NextRecordId = 0;
    createUi();
    createToolBar();
    show();
    //nahození za jízdy, čili za show funguje
    /*QPushButton *button = new QPushButton("Hello");
    layout->addWidget(button);*/
}

//public

void Window_Control_t::setWindowEditorPtr(Window_Editor_t *Window_Editor)
{
    Window_Editor_Ptr = Window_Editor;
}

void Window_Control_t::setWindowVideoPtr(Window_Video_t *Window_Video)
{
    Window_Video_Ptr = Window_Video;
}

void Window_Control_t::setDeafaultMicrophone(){
    newMicrophone();
}

//private

void Window_Control_t::newMicrophone() {
    Microphone *mic1 = new Microphone(this, Window_Video_Ptr, this);
    connect(mic1, SIGNAL(recordingEnd(int, int,int,QString)), Window_Editor_Ptr, SLOT(addNewRecordObject(int, int,int,QString)));
    Layout->addWidget(mic1, 0, 1);
    Layout->setColumnMinimumWidth(1, 100);
}

void Window_Control_t::createUi() {
    setWindowTitle("pDub");
    MainWidget = new QWidget(this);
    this->setCentralWidget(MainWidget);
    Layout = new QGridLayout(MainWidget);
    Layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    Layout->setColumnMinimumWidth(0, 100);
    MainWidget->setLayout(Layout);


    ControlLayout = new QGridLayout();
    Layout->addLayout(ControlLayout, 0, 0);


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
    connect(A_newProject, SIGNAL(triggered()), this, SLOT(newProjectDialog()));

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

void Window_Control_t::newProject(QString projectName, QString videoFilePath, QString projectFolder) {
    ProjectName = projectName;
    VideoFilePath = videoFilePath;
    ProjectFolder = projectFolder;
    Window_Video_Ptr->firstPlay(VideoFilePath);
}

void Window_Control_t::newProjectDialog() {
    // get a path and a name of file
    newProject_dialog *NewProjectDialog = new newProject_dialog(this);
    connect(NewProjectDialog, SIGNAL(accepted(QString, QString, QString)), this, SLOT(newProject(QString, QString, QString)));
    NewProjectDialog->show();
}


void Window_Control_t::loadProject() {

    QString projectFile = QFileDialog::getOpenFileName(this, tr("Open a pDab project"),"", tr("pDab files (*.pDab)"));
    if (projectFile.isEmpty()) {
        //LineEditSelectVideo->setText("Error");
        return;
    }

    QFile file(projectFile);
    QFileInfo fileInfo(file);

    if (!file.open(QIODevice::ReadOnly | QFile::Text))
        return;

    //listOfRecords.clear(); // čištění při opětovném nahrávání
    QXmlStreamReader xmlReader(&file);
    xmlReader.readNextStartElement();  //start <pDab>
    xmlReader.readNextStartElement();  //<nameOfProject>
    ProjectName = xmlReader.readElementText();
    xmlReader.readNextStartElement();  //<projectPath>
    xmlReader.readElementText();
    ProjectFolder = fileInfo.absolutePath();

    RecordPath = ProjectFolder + "/records";
    TmpPath = ProjectFolder + "/tmps";

    xmlReader.readNextStartElement();  //<videoPath>
    /*copyVideo = xmlReader.attributes().value("isCopy").toInt();
    if (copyVideo) {
        m_videoPath = m_projectPath + "/" + xmlReader.readElementText();
    }
    else {*/
        VideoFilePath = xmlReader.readElementText();
    //}
    xmlReader.readNextStartElement();  //první <record>
    int id; int startTime; int endTime; QString name;
    while(!xmlReader.atEnd()) {
        if (xmlReader.isEndElement()) {
            xmlReader.readNext();
            continue;
        }
        id = xmlReader.attributes().value("id").toUInt();
        xmlReader.readNextStartElement();
        name = xmlReader.readElementText();
        xmlReader.readNextStartElement();
        startTime = xmlReader.readElementText().toUInt();
        xmlReader.readNextStartElement();
        /*newRecord.color = xmlReader.readElementText();
        xmlReader.readNextStartElement();
        newRecord.row = xmlReader.readElementText().toInt();
        xmlReader.readNextStartElement();*/
        endTime = xmlReader.readElementText().toUInt();
        xmlReader.readNextStartElement();
        xmlReader.readNextStartElement(); //nevím proč ale jinak to nejde

        Window_Editor_Ptr->addNewRecordObject(id, startTime, endTime, name);
        if (NextRecordId <= id) {
            NextRecordId = id + 1;
        }
    }
    QDir().mkdir(RecordPath);

    /*listOfRecordsSave.clear();
    foreach (int i, listOfRecords.keys()) {
        listOfRecordsSave.insert(i, listOfRecords[i]);
    }*/
    file.close();

    Window_Video_Ptr->firstPlay(VideoFilePath);

}
void Window_Control_t::saveProject() {

    QFile file(ProjectFolder + "/" + ProjectName + ".pDab");
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return;
    }

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("pDab");
    xmlWriter.writeTextElement("projectName", ProjectName);
    xmlWriter.writeTextElement("projectFolder", ProjectFolder);

    xmlWriter.writeStartElement("videoFilePath");
    /*xmlWriter.writeAttribute("isCopy", QString::number(copyVideo));
    if (copyVideo) {
        QFileInfo info(m_videoPath);
        xmlWriter.writeCharacters(info.fileName());
    }
    else {*/
        xmlWriter.writeCharacters(VideoFilePath);
    //}
    xmlWriter.writeEndElement();


    foreach (Record *value, Window_Editor_Ptr->MapRecord) {
        xmlWriter.writeStartElement("record");
        xmlWriter.writeAttribute("id", QString::number(value->Id));
        xmlWriter.writeTextElement("recordName", value->Name);
        xmlWriter.writeTextElement("startTime", QString::number(value->StartTime));
        //xmlWriter.writeTextElement("color", value.color);
        //xmlWriter.writeTextElement("row", QString::number(value.row));
        xmlWriter.writeTextElement("endTime", QString::number(value->EndTime));
        xmlWriter.writeEndElement();
    }
    xmlWriter.writeEndElement();

    xmlWriter.writeEndDocument();
    /*
    listOfRecordsSave.clear();
    foreach (int i, listOfRecords.keys()) {
        listOfRecordsSave.insert(i, listOfRecords[i]);
    }
    */
    file.close();
    if (file.error()) {
        return;
    }

}
void Window_Control_t::exportProject() {
    qDebug() << "export";
}

