#ifndef DATAENTRY_H
#define DATAENTRY_H
#include <QString>
#include <QPointF>

class DataEntry
{

public:
    QString ID;
    long long departureTime;
    long long endTime;
    QPointF orig;
    QPointF dest;
    double fee;

    DataEntry(QString ID, long long departureTime, long long endTime, QPointF orig, QPointF dest, double fee) :
        ID(ID),
        departureTime(departureTime),
        endTime(endTime),
        orig(orig),
        dest(dest),
        fee(fee)
    {}
    DataEntry() {
        ID = "";
        departureTime = 0;
        endTime = 0;
        orig = QPointF(0, 0);
        dest = QPointF(0, 0);
        fee = 0;
    }
    const DataEntry& operator=(const DataEntry& other) {
        if (this == &other) {
            return *this;
        }
        this->ID = other.ID;
        this->departureTime = other.departureTime;
        this->endTime = other.endTime;
        this->orig = other.orig;
        this->dest = other.dest;
        this->fee = other.fee;
        return *this;
    }
};

#endif // DATAENTRY_H
