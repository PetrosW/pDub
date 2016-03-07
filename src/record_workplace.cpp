#include "record_workplace.hpp"

RecordWorkplace::RecordWorkplace(QWidget *parent) : QWidget(parent)
{
    mouseMove = false;
}

void RecordWorkplace::mouseMoveEvent(QMouseEvent *event) {
    mouseMove = true;
}

void RecordWorkplace::mousePressEvent(QMouseEvent *event) {

}

void RecordWorkplace::mouseReleaseEvent(QMouseEvent *event) {
    if (mouseMove == false) {
        sliderPositionChanged(event->pos().x());
    }
    mouseMove = false;
}

