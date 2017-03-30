#include <dialogs/newproject_dialog.hpp>

newProject_dialog::newProject_dialog(QWidget *parent) : QDialog(parent)
{
    this->resize(400,200);
    ButtonSelectVideo = new QPushButton("Select", this);
    ButtonSelectProjectFolder = new QPushButton("Select", this);
    LabelProjectName = new QLabel("Project name:", this);
    LabelSelectVideo = new QLabel("Path to video for your dubbing project:", this);
    LabelSelectProjectFolder = new QLabel("Path to project folder:", this);;
    LineEditProjectName = new QLineEdit("Project_Name", this);
    LineEditSelectVideo = new QLineEdit(this);
    LineEditSelectProjectFolder = new QLineEdit(this);
    ButtonOk = new QPushButton("Ok", this);
    ButtonCancel = new QPushButton("Cancel", this);

    QGridLayout *layout = new QGridLayout(this);
    this->setLayout(layout);
    layout->addWidget(LabelProjectName, 0, 0);
    layout->addWidget(LineEditProjectName, 1, 0);
    layout->addWidget(LabelSelectVideo, 2, 0);
    layout->addWidget(LineEditSelectVideo, 3, 0);
    layout->addWidget(ButtonSelectVideo, 3, 1);
    layout->addWidget(LabelSelectProjectFolder, 4, 0);
    layout->addWidget(LineEditSelectProjectFolder, 5, 0);
    layout->addWidget(ButtonSelectProjectFolder, 5, 1);
    layout->addWidget(ButtonOk, 6, 0);
    layout->addWidget(ButtonCancel, 6, 1);


    connect(ButtonOk, &QPushButton::clicked, this, &newProject_dialog::accept);
    connect(ButtonCancel, &QPushButton::clicked, this, &newProject_dialog::close);
    connect(ButtonSelectProjectFolder, &QPushButton::clicked, this, &newProject_dialog::selectFolder);
    connect(ButtonSelectVideo, &QPushButton::clicked, this, &newProject_dialog::selectVideo);
}

void newProject_dialog::accept() {
    if (!QFileInfo::exists(LineEditSelectProjectFolder->text()) && !QFileInfo::exists(LineEditSelectVideo->text())) {
        //QMessageBox::QMessageBox(QMessageBox::Warning, "Error", "Folder or Video file not exist", QMessageBox::Ok, this, Qt::Dialog);
        QMessageBox::warning(this, "Error", "Folder or Video file not exist", QMessageBox::Ok);
        return;
    }
    this->accepted(LineEditProjectName->text(), LineEditSelectVideo->text(), LineEditSelectProjectFolder->text());
    this->close();
}

void newProject_dialog::selectFolder() {
    QString ProjectFolder = QFileDialog::getExistingDirectory(this, tr("Select project folder"));
    if (ProjectFolder.isEmpty()) {
        LineEditSelectProjectFolder->setText("Error");
        return;
    }
    LineEditSelectProjectFolder->setText(ProjectFolder);
}

void newProject_dialog::selectVideo() {
    QString FileVideo = QFileDialog::getOpenFileName(this, tr("Open a video"),"", tr("Videos (*.avi *.mp4 *.mkv *.webm)"));
    if (FileVideo.isEmpty()) {
        LineEditSelectVideo->setText("Error");
        return;
    }
    LineEditSelectVideo->setText(FileVideo);
}


