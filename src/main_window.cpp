#include <windows/main_window.hpp>

Window_Main_t::Window_Main_t(QWidget *Parent_Ptr) : QMainWindow(Parent_Ptr), Layout_Vertical(new QVBoxLayout() ), Layout_Horizontal(new QHBoxLayout() ),
    Window_Control_Ptr(new Window_Control_t(this) ), Window_Video_Ptr(new Window_Video_t(this) ), Window_Editor_Ptr(new Window_Editor_t(this) )
{
    QWidget *CentralWidget = new QWidget();
    setCentralWidget(CentralWidget);


    Layout_Vertical->setParent(CentralWidget);
    CentralWidget->setLayout(Layout_Vertical);
    Layout_Vertical->addLayout(Layout_Horizontal);

    // pohrat si s velikosti
    Window_Video_Ptr->setMinimumSize(QSize(320, 180));


    QScreen *Screen = QGuiApplication::primaryScreen();
    QSize AvSize = Screen->availableSize();
    qDebug() << AvSize.width();
    qDebug() << AvSize.height();
    this->resize(AvSize.width()*0.90, AvSize.height()*0.90);
    qDebug() << this->frameGeometry().height();
    qDebug() << this->frameGeometry().width();

    Layout_Horizontal->addWidget(Window_Control_Ptr);
    Layout_Horizontal->addWidget(Window_Video_Ptr);
    Layout_Vertical->addWidget(Window_Editor_Ptr);
    Layout_Vertical->setStretch(0, 40);
    Layout_Vertical->setStretch(1, 60);

    Window_Control_Ptr->setWindowVideoPtr(Window_Video_Ptr);
    Window_Control_Ptr->setWindowEditorPtr(Window_Editor_Ptr);

    Window_Video_Ptr->setWindowControlPtr(Window_Control_Ptr);
    Window_Video_Ptr->setWindowEditorPtr(Window_Editor_Ptr);

    Window_Editor_Ptr->setWindowControlPtr(Window_Control_Ptr);
    Window_Editor_Ptr->setWindowVideoPtr(Window_Video_Ptr);

    setWindowTitle("pDub");
    createToolBar();

    Window_Control_Ptr->createAudioEngine(&Window_Editor_Ptr->MapTimeRecord);

    Window_Video_Ptr->createUi();
    Window_Control_Ptr->createUi();
    Window_Editor_Ptr->createUi();

    Window_Control_Ptr->setDeafaultMicrophone();

    connect(Window_Control_Ptr->ButtonDockWindowVideo, &QPushButton::clicked, this, &Window_Main_t::dockingChange);

    show();
}

void Window_Main_t::createToolBar() {

    QMenuBar *MenuBar = new QMenuBar(this);
    this->setMenuBar(MenuBar);


    A_newProject = new QAction(tr("&New"), this);
    A_newProject->setShortcuts(QKeySequence::New);
    A_newProject->setToolTip(tr("Create a new project"));
    connect(A_newProject, &QAction::triggered, this,  &Window_Main_t::newProjectDialog);

    A_loadProject = new QAction(tr("&Open"), this);
    A_loadProject->setShortcuts(QKeySequence::Open);
    A_loadProject->setToolTip(tr("O a old Project"));
    connect(A_loadProject, &QAction::triggered, this, &Window_Main_t::loadProject);

    A_saveProject = new QAction(tr("&Save"), this);
    A_saveProject->setShortcuts(QKeySequence::Save);
    A_saveProject->setToolTip(tr("Save a project"));
    connect(A_saveProject, &QAction::triggered, this, &Window_Main_t::saveProject);

    A_exportProject = new QAction(tr("&Export"), this);
    A_exportProject->setShortcut(tr("Ctrl+E"));
    A_exportProject->setToolTip(tr("Export a project"));
    connect(A_exportProject, &QAction::triggered, this, &Window_Main_t::exportProject);

    A_importAudio = new QAction(tr("&Import"), this);
    A_importAudio->setShortcut(tr("Ctrl+I"));
    A_importAudio->setToolTip(tr("Import own Audio"));
    connect(A_importAudio, &QAction::triggered, this, &Window_Main_t::importAudio);

    MenuBar->addAction(A_newProject);
    MenuBar->addAction(A_loadProject);
    MenuBar->addAction(A_saveProject);
    MenuBar->addAction(A_exportProject);
    MenuBar->addAction(A_importAudio);

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
        Window_Control_Ptr->ButtonDockWindowVideo->setText("Dock Video");
        Window_Video_Ptr->setParent(nullptr);
        Window_Video_Ptr->setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowStaysOnTopHint);
        Window_Video_Ptr->show();
    }
    else
    {
        Window_Video_Ptr->setParent(this);
        Layout_Horizontal->addWidget(Window_Video_Ptr);
        Window_Control_Ptr->ButtonDockWindowVideo->setText("UnDock Video");
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
    Window_Control_Ptr->setProjectName(projectName);
    Window_Control_Ptr->setVideoFilePath(videoFilePath);

    QDir().mkdir(projectFolder);
    QDir().mkdir(Window_Control_Ptr->RecordPath());
    Window_Video_Ptr->firstPlay(videoFilePath);

    foreach (auto map, Window_Editor_Ptr->MapTimeRecord) {
        foreach(Record *item, map) {
            delete item;
        }
        map.clear();
    }
    Window_Editor_Ptr->MapTimeRecord.clear();
    qDebug() << "1";
    qDebug() << videoFilePath;
    Window_Editor_Ptr->m_ffmpeg->convertInputAudio(videoFilePath, Window_Control_Ptr->RecordPath() + "/record0.wav");
    qDebug() << "2";
    Window_Editor_Ptr->addNewRecordObject(Window_Control_Ptr->NextRecordId, Window_Video_Ptr->getPlayerPosition(), 0, "record0.wav", 0, 100);
    qDebug() << "3";
    Window_Control_Ptr->NextRecordId++;



}

void Window_Main_t::newProjectDialog() {
    // get a path and a name of file
    newProject_dialog *NewProjectDialog = new newProject_dialog(this);
    connect(NewProjectDialog, &newProject_dialog::accepted, this, &Window_Main_t::newProject);
//    connect(NewProjectDialog, SIGNAL(accepted(QString, QString, QString)), this, SLOT(newProject(QString, QString, QString)));
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
    Window_Control_Ptr->setProjectName(xmlReader.readElementText());
    qDebug() << "ProjectName " << Window_Control_Ptr->ProjectName();
    xmlReader.readNextStartElement();  //<projectPath>
    xmlReader.readElementText();
    Window_Control_Ptr->setProjectFolder(fileInfo.absolutePath());
    qDebug() << "ProjectFolder " << Window_Control_Ptr->ProjectFolder();

    Window_Control_Ptr->setRecordPath(Window_Control_Ptr->ProjectFolder() + "/records");
    Window_Control_Ptr->setTmpsPath(Window_Control_Ptr->ProjectFolder() + "/tmps");

    xmlReader.readNextStartElement();  //<videoPath>
    /*copyVideo = xmlReader.attributes().value("isCopy").toInt();
    if (copyVideo) {
        m_videoPath = m_projectPath + "/" + xmlReader.readElementText();
    }
    else {*/
        Window_Control_Ptr->setVideoFilePath(xmlReader.readElementText());
    //}
    xmlReader.readNextStartElement();  //první <record>
    uint32_t id; uint32_t startTime; uint32_t endTime; QString name; uint32_t rowPosition; uint32_t volume;
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
        volume = xmlReader.readElementText().toUInt();
        xmlReader.readNextStartElement();
        xmlReader.readNextStartElement(); //nevím proč ale jinak to nejde
        Window_Editor_Ptr->addNewRecordObject(id, startTime, endTime, name, rowPosition, volume);
        if (nextIdLoad <= id) {
            nextIdLoad = id + 1;
        }
    }
    Window_Control_Ptr->NextRecordId = nextIdLoad;
    QDir().mkdir(Window_Control_Ptr->RecordPath());

    /*listOfRecordsSave.clear();
    foreach (int i, listOfRecords.keys()) {
        listOfRecordsSave.insert(i, listOfRecords[i]);
    }*/
    file.close();
    qDebug() << "NextRecordId " << Window_Control_Ptr->NextRecordId;
    qDebug() << "VideoFilePath " << Window_Control_Ptr->VideoFilePath();
    Window_Video_Ptr->firstPlay(Window_Control_Ptr->VideoFilePath());
    //Window_Control_Ptr->updateAudioEngine();

}
void Window_Main_t::saveProject() {

    QFile file(Window_Control_Ptr->ProjectFolder() + "/" + Window_Control_Ptr->ProjectName() + ".pDab");
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return;
    }

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("pDab");
    xmlWriter.writeTextElement("projectName", Window_Control_Ptr->ProjectName());
    xmlWriter.writeTextElement("projectFolder", Window_Control_Ptr->ProjectFolder());

    xmlWriter.writeStartElement("videoFilePath");
    /*xmlWriter.writeAttribute("isCopy", QString::number(copyVideo));
    if (copyVideo) {
        QFileInfo info(m_videoPath);
        xmlWriter.writeCharacters(info.fileName());
    }
    else {*/
        xmlWriter.writeCharacters(Window_Control_Ptr->VideoFilePath());
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
            xmlWriter.writeTextElement("volume", QString::number(item->Volume()));
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
    QString tmpr = Window_Control_Ptr->ProjectFolder() + "/output.mp3";
    try {
        Window_Editor_Ptr->m_ffmpeg->exportProject(Window_Editor_Ptr->MapTimeRecord, Window_Control_Ptr->RecordPath() + "/", tmpr, Window_Control_Ptr->VideoFilePath(), 0, Window_Video_Ptr->videoDuration(), FfmpegExportComponents::AUDIO);
    }
    catch (FfmpegException_t &e) {
        qDebug() << "Export error: " << e.what();
    }

    catch (...) {
        qDebug() << "fail";
    }
}

void Window_Main_t::importAudio() {
    QString audioFile = QFileDialog::getOpenFileName(this, tr("Import Audio"),"", tr("Audio files (*.wav *.mp3 *.flac *.ogg *.m4a);;Video files (*.mkv *.avi *.mp4 *.webm)"));
    if (audioFile.isEmpty()) {
        return;
    }
    qDebug() << "1";
    qDebug() << audioFile;
    qDebug() << Window_Control_Ptr->RecordPath() + "/record" + QString::number(Window_Control_Ptr->NextRecordId) + ".wav";
    Window_Editor_Ptr->m_ffmpeg->convertInputAudio(audioFile, Window_Control_Ptr->RecordPath() + "/record" + QString::number(Window_Control_Ptr->NextRecordId) + ".wav");
    qDebug() << "2";
    Window_Editor_Ptr->addNewRecordObject(Window_Control_Ptr->NextRecordId, Window_Video_Ptr->getPlayerPosition(), 0, "record" + QString::number(Window_Control_Ptr->NextRecordId) + ".wav", 1, 100);
    qDebug() << "3";
    Window_Control_Ptr->NextRecordId++;
}
