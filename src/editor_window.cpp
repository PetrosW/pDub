#include <windows/editor_window.hpp>

Window_Editor_t::Window_Editor_t(Window_Control_t *Window_Control, QWidget *Window_Control_QWidget)
    : QWidget(Window_Control_QWidget),
      Window_Control_Ptr(Window_Control),
      Window_Video_Ptr(nullptr)
{
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint);
    setWindowTitle("Editor");

    QtAV::AVPlayer *recordPlayer = new QtAV::AVPlayer(this);
    VectorMediaPlayer.append(recordPlayer);

    Layout = new QGridLayout(this);
    setLayout(Layout);
    Layout->setVerticalSpacing(0);

    ControlLayout = new QGridLayout(this);
    ControlLayout->setVerticalSpacing(5);
    Layout->addLayout(ControlLayout, 0, 0, 2, 1);
    Layout->setColumnMinimumWidth(0, 200);
    Layout->setColumnStretch(0, 1);

    show();

    TimerNextPlayRecord = new QTimer(this);
    TimerNextPlayRecord->setSingleShot(true);
    TimerNextPlayRecord->setTimerType(Qt::PreciseTimer);
    connect(TimerNextPlayRecord, &QTimer::timeout, this, &Window_Editor_t::updateRecordPlayerTimer);

    m_ffmpeg = new Ffmpeg_t();

    NextPlayingStartTime = 0;
    NextPlayintId = 0;

}

void Window_Editor_t::setWindowVideoPtr(Window_Video_t *Window_Video)
{
    Window_Video_Ptr = Window_Video;

}

void Window_Editor_t::createUi() {
    LabelVideoTime = new QLabel("VideoTime", this);
    ControlLayout->addWidget(LabelVideoTime, 0, 0, 1, 3);

    LabelRecordStartTime = new QLabel("StarTime", this);
    ControlLayout->addWidget(LabelRecordStartTime, 1, 1, 1, 1);
    LabelRecordEndTime = new QLabel("EndTime", this);
    ControlLayout->addWidget(LabelRecordEndTime, 2, 1, 1, 1);

    ButtonBackward = new QPushButton("<<", this);
    ControlLayout->addWidget(ButtonBackward, 1, 0, 2, 1);
    ButtonForward = new QPushButton(">>", this);
    ControlLayout->addWidget(ButtonForward, 1, 2, 2 ,1);

    ButtonSplit = new QPushButton("Split", this);
    ControlLayout->addWidget(ButtonSplit, 3, 0, 1, 3);
    ButtonMerge = new QPushButton("Merge", this);
    ControlLayout->addWidget(ButtonMerge, 4, 0, 1, 3);
    ButtonDelete = new QPushButton("Delete", this);
    ControlLayout->addWidget(ButtonDelete, 5, 0, 1, 3);


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
    connect(ButtonAddRow, SIGNAL(pressed()), this, SLOT(addRow()));

    connect(ScrollBarWorkPlaceHorizontal, &QScrollBar::valueChanged, this, &Window_Editor_t::setScrollAreaEditorTimeSliderValue);
    connect(Window_Video_Ptr, &Window_Video_t::positionVideoChanged, SliderEditorControl, &SliderEditor::setSliderLinePositionFromVideo);
    connect(Window_Video_Ptr, &Window_Video_t::positionVideoChanged, this, &Window_Editor_t::setSliderLinePositionFromVideo);
    connect(this, &Window_Editor_t::sliderLinePositionChanged, Window_Video_Ptr, &Window_Video_t::updateVideoPositionEditorSlider);
    connect(SliderEditorControl, &SliderEditor::sliderLinePositionChanged, this, &Window_Editor_t::setSliderLinePosition);
    connect(WidgetRecordWorkPlace, &RecordWorkplace::sliderPositionChanged, this, &Window_Editor_t::setSliderLinePosition);
    connect(WidgetRecordWorkPlace, &RecordWorkplace::sliderPositionChanged, SliderEditorControl, &SliderEditor::setSliderLinePosition);
    connect(Window_Video_Ptr, &Window_Video_t::signalVideoTimePositionSliderMove, this, &Window_Editor_t::updateRecordPlayer);

    SliderEditorControl->show();
    WidgetWorkPlace->show();

}

//private slots


void Window_Editor_t::addRow() {

//    WidgetWorkPlace->resize(WidgetWorkPlace->width(), WidgetWorkPlace->height()+50);
//    WidgetRecordWorkPlace->resize(WidgetRecordWorkPlace->width(), WidgetRecordWorkPlace->height()+50);
//    ButtonAddRow->move(20,ButtonAddRow->y()+50);
    qDebug() << Window_Control_Ptr->pos();
    qDebug() << Window_Video_Ptr->pos();
    qDebug() << this->pos();
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
    foreach (auto map, MapTimeRecord) {
        foreach(Record *item, map) {
            qDebug() << item->Id() << " : " << item->StartTime();
        }
    }
}

void Window_Editor_t::setSliderLinePosition(uint32_t pos) {
    qDebug() << "lol";
    sliderLinePositionChanged(pos);
    SliderLine->move(pos, 0);
    updateRecordPlayer();
}


void Window_Editor_t::setScrollAreaEditorTimeSliderValue(int value) {
    qDebug() << "lol1";
    ScrollBarEditorTimeSliderHorizontal->setValue(value);
}

void Window_Editor_t::setSliderLinePositionFromVideo(qint64 pos) {
    qDebug() << "lol2";
    SliderLine->move(pos/100, 0);
}

void Window_Editor_t::updateRecordPlayer() {
    TimerNextPlayRecord->stop();
    foreach(QtAV::AVPlayer * recordPlayer, VectorMediaPlayer) {
        recordPlayer->stop();
    }
    uint32_t currentVideoPosition = Window_Video_Ptr->getPlayerPosition();
    if (MapTimeRecord.upperBound(currentVideoPosition) != MapTimeRecord.end()) {
        NextPlayingStartTime = MapTimeRecord.upperBound(currentVideoPosition).value().first()->StartTime();
        NextPlayintId = MapTimeRecord.upperBound(currentVideoPosition).value().first()->Id();
        uint32_t nextTime = NextPlayingStartTime - currentVideoPosition;
        if (!Window_Video_Ptr->isPaused()) {
            TimerNextPlayRecord->setInterval(nextTime);
            TimerNextPlayRecord->start();
        }
    }
    else {
        return;
    }
//    qDebug() << "currentPos " << currentVideoPosition;
//    qDebug() << "nextTime " << NextPlayingStartTime;

    foreach (auto map, MapTimeRecord) {
        foreach(Record *item, map) {
            if (item->StartTime() > currentVideoPosition) {
                break;
            }
            if (item->EndTime() > currentVideoPosition) {
                foreach(QtAV::AVPlayer * recordPlayer, VectorMediaPlayer) {
                    if (!recordPlayer->isPlaying()) {
                        recordPlayer->setFile(Window_Control_Ptr->RecordPath() + "//" + item->Name());
                        recordPlayer->setStartPosition(qint64(currentVideoPosition - item->StartTime()));
                        recordPlayer->audio()->setVolume(0.8);
                        if (!Window_Video_Ptr->isPaused()) {
                            qDebug() << "je pa";
                            recordPlayer->play();
                        }
                        break;
                    }
                    else {
                        QtAV::AVPlayer *newRecordPlayer = new QtAV::AVPlayer(this);
                        VectorMediaPlayer.append(newRecordPlayer);
                        newRecordPlayer->setFile(Window_Control_Ptr->RecordPath() + "//" + item->Name());
                        recordPlayer->setStartPosition(qint64(currentVideoPosition - item->StartTime()));
                        newRecordPlayer->audio()->setVolume(0.8);
                        if (!Window_Video_Ptr->isPaused()) {
                            newRecordPlayer->play();
                        }
                        break;
                    }
                }
            }
        }
    }
}

void Window_Editor_t::updateRecordPlayerTimer() {
    // když jich je více v jednom čase
    TimerNextPlayRecord->stop();
    qDebug() << "fire";
    uint32_t currentVideoPosition = Window_Video_Ptr->getPlayerPosition();

    foreach(QtAV::AVPlayer * recordPlayer, VectorMediaPlayer) {
        if (!recordPlayer->isPlaying()) {
            recordPlayer->setFile(Window_Control_Ptr->RecordPath() + "//" + MapTimeRecord[NextPlayingStartTime].first()->Name());
            recordPlayer->audio()->setVolume(0.8);
            recordPlayer->play();
            break;
        }
        else {
            QtAV::AVPlayer *newRecordPlayer = new QtAV::AVPlayer(this);
            VectorMediaPlayer.append(newRecordPlayer);
            newRecordPlayer->setFile(Window_Control_Ptr->RecordPath() + "//" + MapTimeRecord[NextPlayingStartTime].first()->Name());
            newRecordPlayer->audio()->setVolume(0.8);
            newRecordPlayer->play();
            break;
        }
    }

    NextPlayingStartTime++;
    if (MapTimeRecord.upperBound(NextPlayingStartTime) != MapTimeRecord.end()) {
        NextPlayintId = MapTimeRecord.upperBound(NextPlayingStartTime).value().first()->Id();
        NextPlayingStartTime = MapTimeRecord.upperBound(NextPlayingStartTime).value().first()->StartTime();
        uint32_t nextTime = NextPlayingStartTime - currentVideoPosition;
        if (!Window_Video_Ptr->isPaused()) {
            TimerNextPlayRecord->setInterval(nextTime);
            TimerNextPlayRecord->start();
        }
    }
    //qDebug() << "nextTime" << NextPlayingStartTime;
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

void Window_Editor_t::addNewRecordObject(uint32_t RecordId, uint32_t StartTime, uint32_t EndTime, QString Name) {
    Record *record = new Record(RecordId, StartTime, EndTime, Name, WidgetRecordWorkPlace);
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

    MapRecord.insert(RecordId, record);
    Window_Control_Ptr->NextRecordId++;

}

void Window_Editor_t::videoPausePlayFromVideo(bool isPause) {
    if (isPause) {
        TimerNextPlayRecord->stop();
        foreach(QtAV::AVPlayer *recordPlayer, VectorMediaPlayer) {
            recordPlayer->pause(true);
        }
    }
    else {
        foreach(QtAV::AVPlayer *recordPlayer, VectorMediaPlayer) {
            if (recordPlayer->isPlaying()) {
                recordPlayer->pause(false);
            }
        }
        if (MapTimeRecord.contains(NextPlayingStartTime)) {
            uint32_t currentVideoPosition = Window_Video_Ptr->getPlayerPosition();
            uint32_t nextTime = NextPlayingStartTime - currentVideoPosition;
//            qDebug() << "currentPos " << currentVideoPosition;
//            qDebug() << "nextTime " << NextPlayingStartTime;
            TimerNextPlayRecord->setInterval(nextTime);
            TimerNextPlayRecord->start();
        }
    }
}


