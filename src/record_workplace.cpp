#include "record_workplace.hpp"

RecordWorkplace::RecordWorkplace(QWidget *parent) : QWidget(parent)
{

}

void RecordWorkplace::mouseMoveEvent(QMouseEvent *event) {

}

void RecordWorkplace::mousePressEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::LeftButton) {
        sliderPositionChanged(event->pos().x());
    }
}

void RecordWorkplace::mouseReleaseEvent(QMouseEvent *event) {

}

