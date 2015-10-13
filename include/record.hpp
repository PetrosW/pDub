#ifndef RECORD_HPP
#define RECORD_HPP

#include <QWidget>

#include <common.hpp>


class Record : public QWidget
{
    Q_OBJECT

    public:
        Record();
        ~Record();

    private:

        int Id;
        int StartTime;
        int EndTime;
        QString Name;

};



#endif // RECORD_HPP

