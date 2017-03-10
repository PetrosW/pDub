#include "slider_editor.hpp"

SliderEditor::SliderEditor(QWidget *parent) : QWidget(parent)
{
    SliderClickArea = new QWidget(this);
    SliderClickArea->setGeometry(-5, 0, 11, 10);
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, QColor(255,0,0));
    SliderClickArea->setAutoFillBackground(true);
    SliderClickArea->setPalette(Pal);

    SliderLine = new QWidget(this);
    SliderLine->setGeometry(0, 0, 1, 30);
    SliderLine->setAutoFillBackground(true);
    SliderLine->setPalette(Pal);

    isPressedSliderClickArea = false;
}

// private slots

void SliderEditor::move() {

}

// public slots

void SliderEditor::setSliderLinePosition(uint32_t pos){
    SliderClickArea->move(pos-5, 0);
    SliderLine->move(pos, 0);
}

void SliderEditor::setSliderLinePositionFromVideo(qint64 pos) {
    SliderClickArea->move((pos/100)-5, 0);
    SliderLine->move(pos/100, 0);
    //sliderLinePositionChanged(SliderLine->x());
}

void SliderEditor::mouseMoveEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::LeftButton) {
        if(isPressedSliderClickArea == true) {
            SliderClickArea->move(event->pos().x(), 0);
            SliderLine->move(event->pos().x()+5, 0);
            sliderLinePositionChanged(SliderLine->x());
        }
    }
}

void SliderEditor::mousePressEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::LeftButton) {
        if ((SliderClickArea->x() <= event->pos().x()) && ((SliderClickArea->x()+SliderClickArea->width()) >= event->pos().x())) {
            isPressedSliderClickArea = true;
        }
        else {
            SliderClickArea->move(event->pos().x(), 0);
            SliderLine->move(event->pos().x()+5, 0);
            sliderLinePositionChanged(SliderLine->x());
        }
    }
}

void SliderEditor::mouseReleaseEvent(QMouseEvent *event) {
    // mělo by tam být ale nejde to :D
    //if(event->buttons() & Qt::LeftButton) {
    isPressedSliderClickArea = false;
}

SliderEditor::~SliderEditor()
{

}

