#ifndef RECORDWORKPLACE_H
#define RECORDWORKPLACE_H

#include <QWidget>
#include <QMouseEvent>
#include <common.hpp>

class RecordWorkplace : public QWidget
{
    Q_OBJECT
    public:
        explicit RecordWorkplace(QWidget *parent = 0);
    private:
        bool mouseMove;

    signals:
        void sliderPositionChanged(uint32_t pos);

    public slots:

    private slots:
        void mouseMoveEvent(QMouseEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
};

#endif // RECORDWORKPLACE_H
