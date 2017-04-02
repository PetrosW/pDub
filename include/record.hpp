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
    Q_PROPERTY(uint32_t Volume READ Volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(float VolumeNormalized READ VolumeNormalized WRITE setVolumeNormalized NOTIFY volumeNormalizedChanged)

    public:
        Record(uint32_t id, uint32_t startTime, uint32_t endTime, QString name, uint32_t rowPosition, uint32_t Volume, QWidget *parent = 0);
        ~Record();

        uint32_t Id() { return m_Id; }
        uint32_t StartTime() { return m_StartTime; }
        uint32_t EndTime() { return m_EndTime; }
        QString Name() {return m_Name; }
        uint32_t RowPosition() { return m_RowPosition; }
        uint32_t Duration() { return m_Duration; }
        uint32_t Volume() { return m_Volume; }
        float VolumeNormalized() { return m_VolumeNormalized; }

        void createWaveFormPic(Ffmpeg_t *ffmpeg, QString recortPath);
        void setVolume(const uint32_t &a) { m_Volume = a; m_VolumeNormalized = float(a/100.0f); volumeChanged(); }

    private:

        int dragStartPositionX;
        int dragMouseOffsetX;
        bool mouseMove;

        uint32_t m_Id;
        uint32_t m_StartTime;
        uint32_t m_EndTime;
        QString m_Name;
        uint32_t m_RowPosition;
        uint32_t m_Duration;
        uint32_t m_Volume;
        float m_VolumeNormalized;
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
        void setVolumeNormalized(const float &a) { m_VolumeNormalized = a; volumeNormalizedChanged(); }


    signals:
        void idChanged();
        void startTimeChanged();
        void endTimeChanged();
        void nameChanged();
        void rowPositionChanged();
        void durationChanged();
        void volumeChanged();
        void volumeNormalizedChanged();
        void relocateByMouseMove(uint32_t id, uint32_t oldStartTime);

        void onMouseRelease(Record* Rec, uint32_t RecordID, uint32_t StartTime, uint32_t EndTime, QString Name, uint32_t Volume);
        void onMousePress(Record* Rec, uint32_t RecordID, uint32_t StartTime, uint32_t EndTime, QString Name, uint32_t Volume);
        void onMouseMove(uint32_t RecordID, uint32_t StartTime, uint32_t EndTime, QString Name);

    public slots:
        void select();
        void deselect();
        bool isMuted();

    private slots:
        void mouseMoveEvent(QMouseEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);


};



#endif // RECORD_HPP

