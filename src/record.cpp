#include <record.hpp>

Record::Record(int id, int startTime, int endTime, QString name, QWidget *parent) :
    QWidget(parent), EndTime(endTime), Id(id), Name(name), StartTime(startTime)
{
    qDebug() << "record object";
    qDebug() << parent;
    qDebug() << Id;
    qDebug() << StartTime;
    qDebug() << EndTime;
    qDebug() << Name;


    this->setGeometry(30*Id, 0, 20, 20);
    this->setToolTip("StartTime: " + QString::number(StartTime));

    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, Qt::black);
    this->setAutoFillBackground(true);
    this->setPalette(Pal);
}

Record::~Record()
{

}

