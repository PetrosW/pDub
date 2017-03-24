#include <record.hpp>

Record::Record(uint32_t id, uint32_t startTime, uint32_t endTime, QString name, uint32_t rowPosition, QWidget *parent) :
    m_Id(id), m_StartTime(startTime), m_EndTime(endTime), m_Name(name), m_RowPosition(rowPosition), QWidget(parent)
{
    qDebug() << "record object";
    qDebug() << parent;
    qDebug() << m_Id;
    qDebug() << m_StartTime;
    qDebug() << m_EndTime;
    qDebug() << m_Name;


    this->setGeometry(m_StartTime / 100, m_RowPosition * 50, (m_EndTime - m_StartTime) / 100, 50);
    this->setToolTip("Name: " + m_Name + "\n" + "StartTime: " + QString::number(m_StartTime) + "\n" + "EndTime: " + QString::number(m_EndTime));
    this->setToolTipDuration(-1);

    Palette = new QPalette(palette());
    Palette->setColor(QPalette::Background, Qt::darkRed);
    this->setAutoFillBackground(true);
    this->setPalette(*Palette);
    mouseMove = false;

    m_Duration = m_EndTime - m_StartTime;
    m_WavePic = new QLabel(this);

    CheckBoxMuteRecord = new QCheckBox(this);
    CheckBoxMuteRecord->setChecked(true);

}

void Record::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton) {
        oldStartTime = m_StartTime;
        onMousePress(m_Id, m_StartTime, m_EndTime, m_Name);
        this->raise();
        dragMouseOffsetX = event->pos().x();

        Palette->setColor(QPalette::Background, Qt::blue);
        this->setPalette(*Palette);
    }
}

void Record::mouseMoveEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::LeftButton) {
        if (event->pos().y() > 50){
            QPoint movePoint(this->x(), this->y() + 50);
            m_RowPosition++;
            if (movePoint.y() < 0 || (movePoint.y() + this->height()) > parentWidget()->height())
                return;
            this->move(movePoint);
        }
        else if (event->pos().y() < 0) {
            QPoint movePoint(this->x(), this->y() - 50);
            m_RowPosition--;
            if (movePoint.y() < 0 || (movePoint.y() + this->height()) > parentWidget()->height())
                return;
            this->move(movePoint);
        }
        else {
            QPoint movePoint(this->x() + (event->pos().x() - dragMouseOffsetX), this->y());
            if (movePoint.x() < 0 || (movePoint.x() + this->width()) > parentWidget()->width())
                return;
            this->move(movePoint);
            m_StartTime = this->x() * 100;
            m_EndTime = (m_StartTime + m_Duration);
            onMouseMove(m_Id, m_StartTime, m_EndTime, m_Name);
        }
        mouseMove = true;
    }
    event->ignore();
}

void Record::mouseReleaseEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton) {
        // konstatnta 100 == aktuální zoom

        if (m_StartTime != oldStartTime) {
            relocateByMouseMove(m_Id, oldStartTime);
        }
        onMouseRelease(m_Id, m_StartTime, m_EndTime, m_Name);
    }
    this->setToolTip("Name: " + m_Name + "\n" + "StartTime: " + QString::number(m_StartTime) + "\n" + "EndTime: " + QString::number(m_EndTime));
    this->setToolTipDuration(0);
    if (mouseMove) {
        mouseMove = false;
    }
    else {
        event->ignore();
    }

}

void Record::createWaveFormPic(Ffmpeg_t *ffmpeg, QString recortPath) {

    std::pair<std::vector<double>, std::vector<double> > dataWaveForm = ffmpeg->getSamplesForWaveformPlotting(recortPath + "/" + m_Name);
    QCustomPlot Plotter;
    Plotter.setBackground(QBrush(Qt::transparent) );
    Plotter.xAxis->setVisible(false);
    Plotter.yAxis->setVisible(false);
    Plotter.axisRect()->setAutoMargins(QCP::msNone);
    Plotter.axisRect()->setMargins(QMargins(0, 5, 0, 5));
    QCPGraph *Waveform = Plotter.addGraph();
    Waveform->setPen(QPen(Qt::green) );

    if (!Waveform)
    {
        qDebug("addGraph failed\n");
    }

    QVector<double> Amplitudes(QVector<double>::fromStdVector(dataWaveForm.first) );
    QVector<double> Time;

    double CurrentTime = 0;
    auto TimeSlotCount = Amplitudes.size();

    for (int64_t i = 1; i < TimeSlotCount; i++)
    {
        Time.append(CurrentTime);
        CurrentTime += 0.5;
    }

    Waveform->setData(Time, Amplitudes);
    Plotter.xAxis->setRange(0, Time.back() );
    Plotter.yAxis->setRange(SHRT_MIN, SHRT_MAX);

    QByteArray ByteArray;
    QBuffer Buffer(&ByteArray);
    Buffer.open(QBuffer::WriteOnly);
    uint32_t time = m_EndTime - m_StartTime;
    for (int i = 1; i < 10000; i*=10) {
        Plotter.toPixmap(time/(i), this->height()).save(&Buffer, "PNG", 0);
        //Plotter.saveJpg(recortPath + "/plot" + QString::number(m_Id) + QString::number(i) + ".jpg", time/(i), this->height());

        QPixmap Pixmap;
        Pixmap.loadFromData(ByteArray, "PNG");
        v_PixWaves.append(Pixmap);

        ByteArray.clear();
        Buffer.reset();
    }
    Buffer.close();
    qDebug() << m_WavePic->margin();
    // místo 2 podle toho jaký zoom
    m_WavePic->setPixmap(v_PixWaves[2]);

}

void Record::deselect() {
    Palette->setColor(QPalette::Background, Qt::darkRed);
    this->setPalette(*Palette);
}

bool Record::isMuted() {
    return !CheckBoxMuteRecord->isChecked();
}

Record::~Record()
{

}

