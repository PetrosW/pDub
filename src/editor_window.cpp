#include <windows/editor_window.hpp>

Window_Editor_t::Window_Editor_t(QWidget *parent) : QWidget(parent), Window_Control_Ptr(nullptr), Window_Video_Ptr(nullptr)
{
    //setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint);
    //setWindowTitle("Editor");

    Layout = new QGridLayout();
    setLayout(Layout);
    Layout->setVerticalSpacing(0);

    ControlLayout = new QGridLayout();
    ControlLayout->setVerticalSpacing(5);
    Layout->addLayout(ControlLayout, 0, 0, 2, 1);
    Layout->setColumnMinimumWidth(0, 200);
    Layout->setColumnStretch(0, 1);

    SelectedRecord = NULL;

    m_ffmpeg = new Ffmpeg_t();

}

void Window_Editor_t::setWindowControlPtr(Window_Control_t *Window_Control)
{
    this->Window_Control_Ptr = Window_Control;
}

void Window_Editor_t::setWindowVideoPtr(Window_Video_t *Window_Video)
{
    this->Window_Video_Ptr = Window_Video;

}

void Window_Editor_t::createUi() {


    CheckBoxMuteRecords = new QCheckBox("Mute Records", this);
    ControlLayout->addWidget(CheckBoxMuteRecords, 0, 0, 1, 3);



    LabelRecordStartTime = new QLabel("StarTime: 00:00:00.00", this);
    ControlLayout->addWidget(LabelRecordStartTime, 1, 0, 1, 3);

    ButtonBackward = new QPushButton("<<", this);
    ControlLayout->addWidget(ButtonBackward, 2, 0, 2, 1);
    ButtonForward = new QPushButton(">>", this);
    ControlLayout->addWidget(ButtonForward, 2, 2, 2, 1);

    LabelRecordEndTime = new QLabel("EndTime: 00:00:00.00", this);
    ControlLayout->addWidget(LabelRecordEndTime, 3, 0, 1, 3);

    SliderRecordVolume = new QSlider(Qt::Horizontal, this);
//    SliderRecordVolume->setMaximumWidth(100);
    SliderRecordVolume->setMinimum(0);
    SliderRecordVolume->setMaximum(100);
    SliderRecordVolume->setTickInterval(1);
    SliderRecordVolume->setValue(100);
    ControlLayout->addWidget(SliderRecordVolume, 4, 0, 1, 3);
    connect(SliderRecordVolume, &QSlider::valueChanged, this, &Window_Editor_t::setRecordVolume);

    ButtonSplit = new QPushButton("Split", this);
    ControlLayout->addWidget(ButtonSplit, 5, 0, 1, 3);
    ButtonMerge = new QPushButton("Merge", this);
    ControlLayout->addWidget(ButtonMerge, 6, 0, 1, 3);
    ButtonDelete = new QPushButton("Delete", this);
    ControlLayout->addWidget(ButtonDelete, 7, 0, 1, 3);
    connect(ButtonSplit, &QPushButton::clicked, this, &Window_Editor_t::splitRecord);
    connect(ButtonDelete, &QPushButton::clicked, this, &Window_Editor_t::deleteRecord);

    ScrollAreaEditorTimeSlider = new QScrollArea(this);
    ScrollBarEditorTimeSliderHorizontal = new QScrollBar(ScrollAreaEditorTimeSlider);
    ScrollAreaEditorTimeSlider->setHorizontalScrollBar(ScrollBarEditorTimeSliderHorizontal);
    ScrollAreaEditorTimeSlider->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ScrollAreaEditorTimeSlider->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ScrollAreaEditorTimeSlider->setMinimumHeight(30);
    ScrollAreaEditorTimeSlider->setMaximumHeight(30);

    QPalette Pal2(palette());
    Pal2.setColor(QPalette::Background, QColor(180,100,120));

    SliderEditorControl = new SliderEditor(this);
    SliderEditorControl->setAutoFillBackground(true);
    SliderEditorControl->setPalette(Pal2);

    ScrollAreaEditorTimeSlider->setWidget(SliderEditorControl);

    Layout->addWidget(ScrollAreaEditorTimeSlider, 0, 1);
    Layout->setRowMinimumHeight(0, 30);
    Layout->setRowStretch(0, 1);

    ScrollAreaWorkPlace = new QScrollArea(this);
    ScrollBarWorkPlaceVertical = new QScrollBar(ScrollAreaWorkPlace);
    ScrollAreaWorkPlace->setVerticalScrollBar(ScrollBarWorkPlaceVertical);
    ScrollBarWorkPlaceHorizontal = new QScrollBar(ScrollAreaWorkPlace);
    ScrollAreaWorkPlace->setHorizontalScrollBar(ScrollBarWorkPlaceHorizontal);
    ScrollAreaWorkPlace->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ScrollAreaWorkPlace->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    //connect(ScrollBarWorkPlaceHorizontal, SIGNAL(sliderMoved(int)), this, SLOT(setScrollAreaEditorTimeSliderValue(int)));


    WidgetWorkPlace = new QWidget(this);

    ScrollAreaWorkPlace->setWidget(WidgetWorkPlace);

    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, QColor(80,100,120));

    WidgetRecordWorkPlace = new RecordWorkplace(WidgetWorkPlace);
    WidgetRecordWorkPlace->setAutoFillBackground(true);
    WidgetRecordWorkPlace->setPalette(Pal);

    Layout->addWidget(ScrollAreaWorkPlace, 1, 1);
    Layout->setColumnStretch(1, 9);
    Layout->setRowStretch(1, 99);

    SliderLine = new QWidget(WidgetWorkPlace);
    QPalette Pal3(palette());
    Pal3.setColor(QPalette::Background, QColor(255,0,0));
    SliderLine->setAutoFillBackground(true);
    SliderLine->setPalette(Pal3);


    ButtonAddRow = new QPushButton("Test", WidgetWorkPlace);
    ButtonAddRow->resize(40,40);
    ButtonAddRow->move(20,200 + 10);
    connect(ButtonAddRow, &QPushButton::clicked, this, &Window_Editor_t::addRow);

    connect(ScrollBarWorkPlaceHorizontal, &QScrollBar::valueChanged, this, &Window_Editor_t::setScrollAreaEditorTimeSliderValue);
    connect(Window_Video_Ptr, &Window_Video_t::positionVideoChanged, SliderEditorControl, &SliderEditor::setSliderLinePositionFromVideo);
    connect(Window_Video_Ptr, &Window_Video_t::positionVideoChanged, this, &Window_Editor_t::setSliderLinePositionFromVideo);
    connect(this, &Window_Editor_t::sliderLinePositionChanged, Window_Video_Ptr, &Window_Video_t::updateVideoPositionEditorSlider);
    connect(SliderEditorControl, &SliderEditor::sliderLinePositionChanged, this, &Window_Editor_t::setSliderLinePosition);
    connect(WidgetRecordWorkPlace, &RecordWorkplace::sliderPositionChanged, this, &Window_Editor_t::setSliderLinePosition);
    connect(WidgetRecordWorkPlace, &RecordWorkplace::sliderPositionChanged, SliderEditorControl, &SliderEditor::setSliderLinePosition);
    //connect(Window_Video_Ptr, &Window_Video_t::signalVideoTimePositionSliderMove, this, &Window_Editor_t::updateRecordPlayer);
    //connect(CheckBoxMuteRecords, &QCheckBox::stateChanged, this, &Window_Editor_t::updateRecordPlayer);



//    connect(SliderEditorControl, &SliderEditor::sliderLinePositionChanged, Window_Control_Ptr->AudioPlayback, &AudioPlayback_t::seek);
//    connect(WidgetRecordWorkPlace, &RecordWorkplace::sliderPositionChanged, Window_Control_Ptr->AudioPlayback, &AudioPlayback_t::seek);

    SliderEditorControl->show();
    WidgetWorkPlace->show();

}

//private slots


void Window_Editor_t::addRow() {

//    WidgetWorkPlace->resize(WidgetWorkPlace->width(), WidgetWorkPlace->height()+50);
//    WidgetRecordWorkPlace->resize(WidgetRecordWorkPlace->width(), WidgetRecordWorkPlace->height()+50);
//    ButtonAddRow->move(20,ButtonAddRow->y()+50);
    qDebug() << SelectedRecord->Volume();
    qDebug() << SelectedRecord->VolumeNormalized();
}

void Window_Editor_t::relocateRecordInMap(uint32_t RecordID, uint32_t OldStartTime) {

    if (MapTimeRecord.contains(MapTimeRecord[OldStartTime][RecordID]->StartTime())) {
        MapTimeRecord[MapTimeRecord[OldStartTime][RecordID]->StartTime()].insert(RecordID, MapTimeRecord[OldStartTime][RecordID]);
        MapTimeRecord[OldStartTime].remove(RecordID);
    }
    else {
        QMap<uint32_t, Record *> MapRecord;
        MapRecord.insert(RecordID, MapTimeRecord[OldStartTime][RecordID]);
        MapTimeRecord.insert(MapTimeRecord[OldStartTime][RecordID]->StartTime(), MapRecord);
        MapTimeRecord[OldStartTime].remove(RecordID);
    }
    if (MapTimeRecord[OldStartTime].empty()) {
        MapTimeRecord.remove(OldStartTime);
    }

    Window_Control_Ptr->updateAudioEngine();
//    foreach (auto map, MapTimeRecord) {
//        foreach(Record *item, map) {
//            qDebug() << item->Id() << " : " << item->StartTime();
//        }
//    }
}

void Window_Editor_t::setSliderLinePosition(uint32_t pos) {
    sliderLinePositionChanged(pos);
    SliderLine->move(pos, 0);
}

void Window_Editor_t::setScrollAreaEditorTimeSliderValue(int value) {
    ScrollBarEditorTimeSliderHorizontal->setValue(value);
}

void Window_Editor_t::setSliderLinePositionFromVideo(qint64 pos) {
    SliderLine->move(pos/100, 0);
    Window_Control_Ptr->updateLabelVideoTime(pos);
}

void Window_Editor_t::splitRecord() {

    Window_Control_Ptr->releaseAudioResources();

    // TODO: upravit !
    Record *splitRec = SelectedRecord;

    uint32_t pos = (SliderLine->x()*100) - splitRec->StartTime();
    m_ffmpeg->splitTrack(splitRec, Window_Control_Ptr->RecordPath(), Window_Control_Ptr->NextRecordId, pos);

    for (auto it = MapTimeRecord.begin(); it != MapTimeRecord.end();) {
        if (it.value().contains(SelectedRecordId)) {
            delete it.value()[SelectedRecordId];
            it.value().remove(SelectedRecordId);
            if (it.value().isEmpty()) {
                MapTimeRecord.erase(it);
            }
            break;
        }
        else {
            ++it;
        }
    }

    addNewRecordObject(Window_Control_Ptr->NextRecordId, splitRec->StartTime(), 0, "record"+QString::number(Window_Control_Ptr->NextRecordId)+".wav", splitRec->RowPosition(), splitRec->Volume());
    addNewRecordObject(Window_Control_Ptr->NextRecordId, splitRec->StartTime()+pos+1, 0, "record"+QString::number(Window_Control_Ptr->NextRecordId)+".wav", splitRec->RowPosition(), splitRec->Volume());

}

void Window_Editor_t::deleteRecord() {

    Window_Control_Ptr->releaseAudioResources();

    for (auto it = MapTimeRecord.begin(); it != MapTimeRecord.end();) {
        if (it.value().contains(SelectedRecordId)) {
            delete it.value()[SelectedRecordId];
            it.value().remove(SelectedRecordId);

            if (it.value().isEmpty()) {
                MapTimeRecord.erase(it);
            }

            break;
        }
        else {
            ++it;
        }
    }

    foreach (auto map, MapTimeRecord) {
        foreach(Record *item, map) {
            qDebug() << item->Name();
        }
    }

    Window_Control_Ptr->updateAudioEngine();
}

//protected

void Window_Editor_t::paintEvent(QPaintEvent *)
{

}

//public

void Window_Editor_t::setAfterVideoLoad(qint64 duration) {
    // 200 velikost 200/50 počet řádků potom načíst z uložení
    WidgetWorkPlace->setGeometry(0, 0, int(duration / 100) , 200 + 100);
    SliderEditorControl->setGeometry(0, 0, int(duration / 100) , 30);
    WidgetRecordWorkPlace->setGeometry(0, 0, int(duration / 100) , 200);
    SliderLine->setGeometry(0, 0, 1, 200);
}

//public slots

void Window_Editor_t::addNewRecordObject(uint32_t RecordId, uint32_t StartTime, uint32_t EndTime, QString Name, uint32_t RowPosition, uint32_t Volume) {
    qDebug() << "startTime: "<< StartTime;
    EndTime = StartTime + (uint32_t)m_ffmpeg->getAudioDuration(Window_Control_Ptr->RecordPath() + "/" + Name);
    if (RowPosition == 500) {
        RowPosition = 0;
    }
    Record *record = new Record(RecordId, StartTime, EndTime, Name, RowPosition, Volume, WidgetRecordWorkPlace);
    record->createWaveFormPic(m_ffmpeg, Window_Control_Ptr->RecordPath());
    record->show();

    if (MapTimeRecord.contains(StartTime)) {
        MapTimeRecord[StartTime].insert(RecordId, record);
    }
    else {
        QMap<uint32_t, Record *> MapRecord;
        MapRecord.insert(RecordId, record);
        MapTimeRecord.insert(StartTime, MapRecord);
    }

    connect(record, &Record::relocateByMouseMove, this, &Window_Editor_t::relocateRecordInMap);
    connect(record, &Record::onMouseMove, this, &Window_Editor_t::recordMoveSelected);
    connect(record, &Record::onMousePress, this, &Window_Editor_t::recordSelected);
    record->select();
    Window_Control_Ptr->NextRecordId++;
    Window_Control_Ptr->updateAudioEngine();

}

void Window_Editor_t::recordSelected(Record* RecordPointer, uint32_t RecordId, uint32_t StartTime, uint32_t EndTime, QString Name, uint32_t Volume) {
    if (SelectedRecord != NULL) {
        SelectedRecord->deselect();
    }
    SelectedRecord = RecordPointer;
    SelectedRecordId = RecordId;
    LabelRecordEndTime->setText("EndTime: " + miliSecToTime(EndTime));
    LabelRecordStartTime->setText("StartTime: " + miliSecToTime(StartTime));
    SliderRecordVolume->setValue(Volume);
}

void Window_Editor_t::setRecordVolume() {
    SelectedRecord->setVolume(uint32_t(SliderRecordVolume->value()));
}

void Window_Editor_t::recordMoveSelected(uint32_t RecordId, uint32_t StartTime, uint32_t EndTime, QString Name) {
    LabelRecordEndTime->setText("EndTime: " + miliSecToTime(EndTime));
    LabelRecordStartTime->setText("StartTime: " + miliSecToTime(StartTime));
}


