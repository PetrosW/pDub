#ifndef RECORD_HPP
#define RECORD_HPP

#include <QWidget>
#include <QMouseEvent>
#include <QPixmap>
#include <QByteArray>
#include <QBuffer>
#include <QBrush>
#include <QPen>
#include <QLabel>

#include <utility>
#include <qcustomplot-source/qcustomplot.h>

#include <common.hpp>

class Record;
#include <ffmpeg/ffmpeg.hpp>



class Record : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(uint32_t Id READ Id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(uint32_t StartTime READ StartTime WRITE setStartTime NOTIFY startTimeChanged)
    Q_PROPERTY(uint32_t EndTime READ EndTime WRITE setEndTime NOTIFY endTimeChanged)
    Q_PROPERTY(QString Name READ Name WRITE setName NOTIFY nameChanged)

    public:
        Record(uint32_t id, uint32_t startTime, uint32_t endTime, QString name, QWidget *parent = 0);
        ~Record();

        uint32_t Id() { return m_Id; }
        uint32_t StartTime() { return m_StartTime; }
        uint32_t EndTime() { return m_EndTime; }
        QString Name() {return m_Name; }

        void createWaveFormPic(Ffmpeg_t *ffmpeg, QString recortPath);

    private:

        int dragStartPositionX;
        int dragMouseOffsetX;

        uint32_t m_Id;
        uint32_t m_StartTime;
        uint32_t m_EndTime;
        QString m_Name;

        QVector<QPixmap> v_PixWaves;

        QLabel *m_WavePic;

        void setId(const uint32_t &a) { m_Id = a; idChanged();}
        void setStartTime(const uint32_t &a) { m_StartTime = a; startTimeChanged(); }
        void setEndTime(const uint32_t &a) { m_EndTime = a; endTimeChanged();}
        void setName(const QString &a) { m_Name = a; nameChanged();}

    signals:
        void idChanged();
        void startTimeChanged();
        void endTimeChanged();
        void nameChanged();
        void relocateByMouseMove(uint32_t id, uint32_t oldStartTime);

    public slots:

    private slots:
        void mouseMoveEvent(QMouseEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);


};



#endif // RECORD_HPP

