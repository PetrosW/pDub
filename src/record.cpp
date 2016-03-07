#include <record.hpp>

Record::Record(int id, int startTime, int endTime, QString name, QWidget *parent) :
    Id(id), StartTime(startTime), EndTime(endTime), Name(name), QWidget(parent)
{
    qDebug() << "record object";
    qDebug() << parent;
    qDebug() << Id;
    qDebug() << StartTime;
    qDebug() << EndTime;
    qDebug() << Name;


    this->setGeometry(startTime / 100, 0, (endTime - startTime) / 100, 50);
    this->setToolTip("StartTime: " + QString::number(StartTime) + "\n" + "EndTime: " + QString::number(EndTime));
    this->setToolTipDuration(0);

    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, Qt::darkGreen);
    this->setAutoFillBackground(true);
    this->setPalette(Pal);
}

void Record::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton) {
        this->raise();
        dragStartPositionX = this->x();
        dragMouseOffsetX = event->pos().x();


        QPalette Pal(palette());
        Pal.setColor(QPalette::Background, Qt::blue);
        this->setAutoFillBackground(true);
        this->setPalette(Pal);
    }
    event->ignore();
}

void Record::mouseMoveEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::LeftButton) {
        if (event->pos().y() > 50){
            QPoint movePoint(this->x(), this->y() + 50);
            if (movePoint.y() < 0 || (movePoint.y() + this->height()) > parentWidget()->height())
                return;
            this->move(movePoint);
        }
        else if (event->pos().y() < 0) {
            QPoint movePoint(this->x(), this->y() - 50);
            if (movePoint.y() < 0 || (movePoint.y() + this->height()) > parentWidget()->height())
                return;
            this->move(movePoint);
        }
        else {
            QPoint movePoint(this->x() + (event->pos().x() - dragMouseOffsetX), this->y());
            if (movePoint.x() < 0 || (movePoint.x() + this->width()) > parentWidget()->width())
                return;
            this->move(movePoint);
        }
    }
    event->ignore();
}

void Record::mouseReleaseEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton) {
        // konstatnta 100 == aktuální zoom

        int tmp = ((this->x() - dragStartPositionX) * 100);
        StartTime = StartTime + tmp;
        EndTime = EndTime + tmp;

    }
    this->setToolTip("StartTime: " + QString::number(StartTime) + "\n" + "EndTime: " + QString::number(EndTime));
    this->setToolTipDuration(0);
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, Qt::darkGreen);
    this->setAutoFillBackground(true);
    this->setPalette(Pal);
    event->ignore();
}


Record::~Record()
{

}

