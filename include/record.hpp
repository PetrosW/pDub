#ifndef RECORD_HPP
#define RECORD_HPP

#include <QWidget>
#include <QMouseEvent>

#include <common.hpp>


class Record : public QWidget
{
    Q_OBJECT

    public:
        Record(int id, int startTime, int endTime, QString name, QWidget *parent = 0);
        ~Record();

        int Id;
        int StartTime;
        int EndTime;
        QString Name;

    private:

        int dragStartPositionX;
        int dragMouseOffsetX;

    public slots:
    private slots:
        void mouseMoveEvent(QMouseEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);


};



#endif // RECORD_HPP

