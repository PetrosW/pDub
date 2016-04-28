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
    Q_PROPERTY(uint32_t RowPosition READ RowPosition WRITE setRowPosition NOTIFY rowPositionChanged)
    Q_PROPERTY(uint32_t Duration READ Duration WRITE setDuration NOTIFY durationChanged)

    public:
        Record(uint32_t id, uint32_t startTime, uint32_t endTime, QString name, uint32_t rowPosition, QWidget *parent = 0);
        ~Record();

        uint32_t Id() { return m_Id; }
        uint32_t StartTime() { return m_StartTime; }
        uint32_t EndTime() { return m_EndTime; }
        QString Name() {return m_Name; }
        uint32_t RowPosition() { return m_RowPosition; }
        uint32_t Duration() { return m_Duration; }

        void createWaveFormPic(Ffmpeg_t *ffmpeg, QString recortPath);

    private:

        int dragStartPositionX;
        int dragMouseOffsetX;

        uint32_t m_Id;
        uint32_t m_StartTime;
        uint32_t m_EndTime;
        QString m_Name;
        uint32_t m_RowPosition;
        uint32_t m_Duration;
        uint32_t oldStartTime;

        QVector<QPixmap> v_PixWaves;

        QLabel *m_WavePic;

        QCheckBox *CheckBoxMuteRecord;

        QPalette *Palette;

        void setId(const uint32_t &a) { m_Id = a; idChanged();}
        void setStartTime(const uint32_t &a) { m_StartTime = a; startTimeChanged(); }
        void setEndTime(const uint32_t &a) { m_EndTime = a; endTimeChanged();}
        void setName(const QString &a) { m_Name = a; nameChanged(); }
        void setRowPosition(const uint32_t &a) { m_RowPosition = a; rowPositionChanged(); }
        void setDuration(const uint32_t &a) { m_Duration = a; durationChanged(); }

    signals:
        void idChanged();
        void startTimeChanged();
        void endTimeChanged();
        void nameChanged();
        void rowPositionChanged();
        void durationChanged();
        void relocateByMouseMove(uint32_t id, uint32_t oldStartTime);

        void onMouseRelease(uint32_t RecordID, uint32_t StartTime, uint32_t EndTime, QString Name);
        void onMousePress(uint32_t RecordID, uint32_t StartTime, uint32_t EndTime, QString Name);
        void onMouseMove(uint32_t RecordID, uint32_t StartTime, uint32_t EndTime, QString Name);

    public slots:

        void deselect();
        bool isMuted();

    private slots:
        void mouseMoveEvent(QMouseEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);


};



#endif // RECORD_HPP

