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

    ControlLayout = new QGridLayout();
    Layout->addLayout(ControlLayout, 0, 0);
    Layout->setColumnMinimumWidth(0, 100);
    Layout->setColumnStretch(0, 1);


    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, QColor(80,100,120));

//    Layout->addWidget(WorkPlaceWidget, 0, 1);
//    Layout->setColumnMinimumWidth(0, 200);
//    Layout->setColumnStretch(1, 9);

    WorkPlaceScrollArea = new QScrollArea(this);
    //WorkPlaceScrollArea->setBackgroundRole(QPalette::Dark);
    WorkPlaceScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    WorkPlaceScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    WorkPlaceWidget = new QWidget(this);
    WorkPlaceWidget->setAutoFillBackground(true);
    WorkPlaceWidget->setPalette(Pal);


    WorkPlaceScrollArea->setWidget(WorkPlaceWidget);
    //WorkPlaceScrollArea->setWidgetResizable(true);

    Layout->addWidget(WorkPlaceScrollArea, 0, 1);
    Layout->setColumnMinimumWidth(0, 200);
    Layout->setColumnStretch(1, 9);

//    SliderWidget = new QWidget(this);
//    Layout->addWidget(SliderWidget, 0, 2);
//    Layout->setColumnMinimumWidth(0, 200);
//    Layout->setColumnStretch(1, 9);
//    SliderWidget->setAutoFillBackground(true);
//    SliderWidget->setPalette(Pal);
    WorkPlaceWidget->show();

    ButtonAddRow = new QPushButton("+", WorkPlaceWidget);
    ButtonAddRow->resize(40,40);
    ButtonAddRow->move(20,160);
    connect(ButtonAddRow, SIGNAL(pressed()), this, SLOT(addRow()));
    //lol->show();

    createUi();

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
    ControlLayout->addWidget(LabelRecordStartTime, 1, 1);
    LabelRecordEndTime = new QLabel("EndTime", this);
    ControlLayout->addWidget(LabelRecordEndTime, 2, 1);


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

}

//private slots

void Window_Editor_t::addRow() {
    WorkPlaceWidget->resize(WorkPlaceWidget->width(), WorkPlaceWidget->height()+50);
    ButtonAddRow->move(20,ButtonAddRow->y()+50);
}

//public

void Window_Editor_t::setAfterVideoLoad(qint64 duration) {
    // 200 velikost 200/50 počet řádků potom načíst z uložení
    WorkPlaceWidget->setGeometry(0, 0, int(duration / 100) , 200);
}

//public slots

void Window_Editor_t::addNewRecordObject(int RecordId, int StartTime, int EndTime, QString Name) {
    Record *record = new Record(RecordId, StartTime, EndTime, Name, WorkPlaceWidget);
    record->show();
    MapRecord.insert(RecordId, record);
    Window_Control_Ptr->NextRecordId++;

}
