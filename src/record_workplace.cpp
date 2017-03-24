#include "record_workplace.hpp"

RecordWorkplace::RecordWorkplace(QWidget *parent) : QWidget(parent)
{
}

void RecordWorkplace::mouseMoveEvent(QMouseEvent *event) {
}

void RecordWorkplace::mousePressEvent(QMouseEvent *event) {

}

void RecordWorkplace::mouseReleaseEvent(QMouseEvent *event) {
    sliderPositionChanged(event->pos().x());

}

