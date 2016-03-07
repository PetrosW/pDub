#include <windows/editor_window.hpp>

Window_Editor_t::Window_Editor_t(Window_Control_t *Window_Control, QWidget *Window_Control_QWidget)
    : QWidget(Window_Control_QWidget),
      Window_Control_Ptr(Window_Control),
      Window_Video_Ptr(nullptr)
{
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint);
    setWindowTitle("Editor");

    Layout = new QGridLayout(this);
    setLayout(Layout);
    Layout->setVerticalSpacing(0);

    ControlLayout = new QGridLayout(this);
    Layout->addLayout(ControlLayout, 0, 0, 2, 1);
    Layout->setColumnMinimumWidth(0, 200);
    Layout->setColumnStretch(0, 1);


    show();

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

void Window_Editor_t::setSliderLinePosition(uint32_t pos) {
    sliderLinePositionChanged(pos);
    SliderLine->move(pos, 0);
}


void Window_Editor_t::setScrollAreaEditorTimeSliderValue(int value) {
    ScrollBarEditorTimeSliderHorizontal->setValue(value);
}

void Window_Editor_t::setSliderLinePositionFromVideo(qint64 pos) {
    SliderLine->move(pos/100, 0);
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

void Window_Editor_t::addNewRecordObject(int RecordId, int StartTime, int EndTime, QString Name) {
    Record *record = new Record(RecordId, StartTime, EndTime, Name, WidgetRecordWorkPlace);
    record->show();
    MapRecord.insert(RecordId, record);
    Window_Control_Ptr->NextRecordId++;

}


