#ifndef DATAENTRY_H
#define DATAENTRY_H
#include <QString>
struct Point {
    double lng;
    double lat;
};

class DataEntry
{

public:
    QString ID;
    int departureTime;
    int endTime;
    Point orig;
    Point dest;
    double fee;

    DataEntry(QString ID, int departureTime, int endTime, Point orig, Point dest, double fee) :
        ID(ID),
        departureTime(departureTime),
        endTime(endTime),
        orig(orig),
        dest(dest),
        fee(fee)
    {}
};

#endif // DATAENTRY_H
