#ifndef IMAGEOBJECT_H
#define IMAGEOBJECT_H

#include "MapGraphics_global.h"
#include "Position.h"
#include "MapGraphicsObject.h"

class MAPGRAPHICSSHARED_EXPORT ImageObject : public MapGraphicsObject
{
    Q_OBJECT
public:
    explicit ImageObject(const Position& topLeft, const Position& bottomRight, QImage img, MapGraphicsObject *parent = nullptr);
    virtual ~ImageObject();

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget *w);
signals:

public slots:

private slots:
    void updatePositionFromEndPoints();

private:
    Position topLeft;
    Position bottomRight;
    QImage img;
};

#endif // IMAGEOBJECT_H
