#include <windows/main_window.hpp>

Window_Main_t::Window_Main_t(QWidget *Parent_Ptr) : QMainWindow(Parent_Ptr), Layout_Vertical(new QVBoxLayout() ), Layout_Horizontal(new QHBoxLayout() ),
    Window_Control_Ptr(new Window_Control_t(this) ), Window_Video_Ptr(new Window_Video_t(this) ), Window_Editor_Ptr(new Window_Editor_t(this) )
{
    QWidget *CentralWidget = new QWidget();
    setCentralWidget(CentralWidget);

    qDebug() << "main pre";

    Layout_Vertical->setParent(CentralWidget);
    CentralWidget->setLayout(Layout_Vertical);
    Layout_Vertical->addLayout(Layout_Horizontal);

    // pohrat si s velikosti
    Window_Video_Ptr->setMinimumSize(QSize(640, 480));

    Layout_Horizontal->addWidget(Window_Control_Ptr);
    Layout_Horizontal->addWidget(Window_Video_Ptr);
    Layout_Vertical->addWidget(Window_Editor_Ptr);

    Window_Control_Ptr->setWindowVideoPtr(Window_Video_Ptr);
    Window_Control_Ptr->setWindowEditorPtr(Window_Editor_Ptr);

    Window_Video_Ptr->setWindowControlPtr(Window_Control_Ptr);
    Window_Video_Ptr->setWindowEditorPtr(Window_Editor_Ptr);

    Window_Editor_Ptr->setWindowControlPtr(Window_Control_Ptr);
    Window_Editor_Ptr->setWindowVideoPtr(Window_Video_Ptr);

    qDebug() << "main po";

    setWindowTitle("pDub");
    createToolBar();

    Window_Video_Ptr->createUi();

    Window_Control_Ptr->createUi();

    Window_Editor_Ptr->createUi();

    qDebug() << "microfon pre";
    Window_Control_Ptr->setDeafaultMicrophone();
    qDebug() << "microfon po";

    connect(Window_Control_Ptr->ButtonDockWindowVideo, &QPushButton::clicked, this, &Window_Main_t::dockingChange);

    show();
}

void Window_Main_t::createToolBar() {

    QMenuBar *MenuBar = new QMenuBar(this);
    this->setMenuBar(MenuBar);


    A_newProject = new QAction(tr("&New"), this);
    A_newProject->setShortcuts(QKeySequence::New);
    A_newProject->setToolTip(tr("Create a new project"));
    connect(A_newProject, SIGNAL(triggered()), this, SLOT(newProjectDialog()));

    A_loadProject = new QAction(tr("&Open"), this);
    A_loadProject->setShortcuts(QKeySequence::Open);
    A_loadProject->setToolTip(tr("O a old Project"));
    connect(A_loadProject, SIGNAL(triggered()), this, SLOT(loadProject()));

    A_saveProject = new QAction(tr("&Save"), this);
    A_saveProject->setShortcuts(QKeySequence::Save);
    A_saveProject->setToolTip(tr("Save a project"));
    connect(A_saveProject, SIGNAL(triggered()), this, SLOT(saveProject()));

    A_exportProject = new QAction(tr("&Export"), this);
    A_exportProject->setShortcut(tr("Ctrl+E"));
    A_exportProject->setToolTip(tr("Export a project"));
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

void Window_Main_t::dockingChange()
{
    if (IsDocked)
    {
        //Layout_Horizontal->removeWidget(Window_Video_Ptr);

        Window_Video_Ptr->setParent(nullptr);
        Window_Video_Ptr->setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowStaysOnTopHint);
        Window_Video_Ptr->show();
    }
    else
    {
        Window_Video_Ptr->setParent(this);
        Layout_Horizontal->addWidget(Window_Video_Ptr);
    }

    IsDocked = !IsDocked;
}

void Window_Main_t::closeEvent(QCloseEvent *event)
{
    if (!IsDocked) Window_Video_Ptr->close();
    QMainWindow::closeEvent(event);
}

// private slots

void Window_Main_t::newProject(QString projectName, QString videoFilePath, QString projectFolder) {
    Window_Control_Ptr->setProjectFolder(projectFolder);
    Window_Control_Ptr->setRecordPath(projectFolder + "/records");
    Window_Control_Ptr->setProjectFolder(projectFolder);
    Window_Control_Ptr->setVideoFilePath(videoFilePath);

    QDir().mkdir(projectFolder);
    Window_Video_Ptr->firstPlay(videoFilePath);

    foreach (auto map, Window_Editor_Ptr->MapTimeRecord) {
        foreach(Record *item, map) {
            delete item;
        }
        map.clear();
    }
    Window_Editor_Ptr->MapTimeRecord.clear();
}

void Window_Main_t::newProjectDialog() {
    // get a path and a name of file
    newProject_dialog *NewProjectDialog = new newProject_dialog(this);
    connect(NewProjectDialog, SIGNAL(accepted(QString, QString, QString)), this, SLOT(newProject(QString, QString, QString)));
    NewProjectDialog->show();
}


void Window_Main_t::loadProject() {

    QString projectFile = QFileDialog::getOpenFileName(this, tr("Open a pDab project"),"", tr("pDab files (*.pDab)"));
    if (projectFile.isEmpty()) {
        //LineEditSelectVideo->setText("Error");
        return;
    }

    QFile file(projectFile);
    QFileInfo fileInfo(file);

    if (!file.open(QIODevice::ReadOnly | QFile::Text))
        return;


    foreach (auto map, Window_Editor_Ptr->MapTimeRecord) {
        foreach(Record *item, map) {
            delete item;
        }
        map.clear();
    }
    Window_Editor_Ptr->MapTimeRecord.clear();

    //listOfRecords.clear(); // čištění při opětovném nahrávání
    QXmlStreamReader xmlReader(&file);
    xmlReader.readNextStartElement();  //start <pDab>
    xmlReader.readNextStartElement();  //<nameOfProject>
    ProjectName = xmlReader.readElementText();
    xmlReader.readNextStartElement();  //<projectPath>
    xmlReader.readElementText();
    m_ProjectFolder = fileInfo.absolutePath();

    m_RecordPath = m_ProjectFolder + "/records";
    TmpPath = m_ProjectFolder + "/tmps";

    xmlReader.readNextStartElement();  //<videoPath>
    /*copyVideo = xmlReader.attributes().value("isCopy").toInt();
    if (copyVideo) {
        m_videoPath = m_projectPath + "/" + xmlReader.readElementText();
    }
    else {*/
        VideoFilePath = xmlReader.readElementText();
    //}
    xmlReader.readNextStartElement();  //první <record>
    uint32_t id; uint32_t startTime; uint32_t endTime; QString name; uint32_t rowPosition;
    uint32_t nextIdLoad = 0;
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
        rowPosition = xmlReader.readElementText().toUInt();
        xmlReader.readNextStartElement();
        xmlReader.readNextStartElement(); //nevím proč ale jinak to nejde

        Window_Editor_Ptr->addNewRecordObject(id, startTime, endTime, name, rowPosition);
        if (nextIdLoad <= id) {
            nextIdLoad = id + 1;
        }
    }
    Window_Control_Ptr->NextRecordId = nextIdLoad;
    QDir().mkdir(m_RecordPath);

    /*listOfRecordsSave.clear();
    foreach (int i, listOfRecords.keys()) {
        listOfRecordsSave.insert(i, listOfRecords[i]);
    }*/
    file.close();
    qDebug() << "NextRecordId " << Window_Control_Ptr->NextRecordId;
    Window_Video_Ptr->firstPlay(VideoFilePath);

}
void Window_Main_t::saveProject() {

    QFile file(m_ProjectFolder + "/" + ProjectName + ".pDab");
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return;
    }

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("pDab");
    xmlWriter.writeTextElement("projectName", ProjectName);
    xmlWriter.writeTextElement("projectFolder", m_ProjectFolder);

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

    qDebug() << "save";
    foreach (auto map, Window_Editor_Ptr->MapTimeRecord) {
        qDebug() << "save22";
        foreach(Record *item, map) {
            xmlWriter.writeStartElement("record");
            xmlWriter.writeAttribute("id", QString::number(item->Id()));
            xmlWriter.writeTextElement("recordName", item->Name());
            xmlWriter.writeTextElement("startTime", QString::number(item->StartTime()));
            //xmlWriter.writeTextElement("color", value.color);
            //xmlWriter.writeTextElement("row", QString::number(value.row));
            xmlWriter.writeTextElement("endTime", QString::number(item->EndTime()));
            xmlWriter.writeTextElement("rowPosition", QString::number(item->RowPosition()));
            xmlWriter.writeEndElement();
        }
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
void Window_Main_t::exportProject() {
    QString tmpr = m_ProjectFolder + "/output.mp3";
    try {
        Window_Editor_Ptr->m_ffmpeg->exportProject(Window_Editor_Ptr->MapTimeRecord, m_RecordPath + "/", tmpr, VideoFilePath, 0, Window_Video_Ptr->videoDuration(), FfmpegExportComponents::AUDIO);
    }
    catch (FfmpegException_t &e) {
        qDebug() << "Export error: " << e.what();
    }

    catch (...) {
        qDebug() << "fail";
    }
}
