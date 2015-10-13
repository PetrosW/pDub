#include <common.hpp>

QString miliSecToTime(int miliSec) {
    return QString("%1:%2:%3")
            .arg(qFloor(miliSec / 1000 / 60 / 60), 2, 10, QChar('0'))
            .arg(qFloor(miliSec / 1000 / 60) % 60, 2, 10, QChar('0'))
            .arg(fmod(double(miliSec) / 1000.0, 60), 5, 'f', 2, QChar('0'));
}
