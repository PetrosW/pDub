#ifndef SLIDEREDITOR_HPP
#define SLIDEREDITOR_HPP

#include <QWidget>
#include <QtGui>
#include <QMouseEvent>

#include <common.hpp>

class SliderEditor : public QWidget
{
    Q_OBJECT

    public:
        SliderEditor(QWidget *parent = 0);
        ~SliderEditor();
    private:
        QWidget *SliderClickArea;
        QWidget *SliderLine;

        bool isPressedSliderClickArea;

    signals:
        void sliderLinePositionChanged(uint32_t pos);

    public slots:
        void setSliderLinePosition(uint32_t pos);
        void setSliderLinePositionFromVideo(qint64 pos);

    private slots:
        void mouseMoveEvent(QMouseEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);

        void move();
};

#endif // SLIDEREDITOR_HPP
