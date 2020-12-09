#ifndef POINTOBJECT_H
#define POINTOBJECT_H

#include "MapGraphics_global.h"
#include "Position.h"
#include "MapGraphicsObject.h"
#include <QDebug>

class MAPGRAPHICSSHARED_EXPORT PointObject : public MapGraphicsObject
{
    Q_OBJECT
public:
    explicit PointObject(const Position& p, qreal size, qreal thickness = 1.0, QColor color = QColor(255, 0, 0, 100), MapGraphicsObject* parent = 0);
    virtual ~PointObject();

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget *widget);

    qreal thickness() const;
    void setThickness(qreal nThick);

    QColor brushColor() const;
    void setBrushColor(QColor mColor);
signals:

public slots:
    void setPoint(const Position& pos);

private slots:
    void updatePositionFromPoint();

private:
    Position _p;
    qreal _thickness;
    qreal _size;
    QColor _brushColor = QColor(255, 0, 0, 100);
};

#endif // POINTOBJECT_H
