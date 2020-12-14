#include "ImageObject.h"

#include "guts/Conversions.h"
#include <QtGlobal>


ImageObject::ImageObject(const Position& topLeft, const Position& bottomRight, QImage img, MapGraphicsObject *parent) :
    MapGraphicsObject(false, parent),
    topLeft(topLeft),
    bottomRight(bottomRight),
    img(img.copy(img.rect()))
{
   this->updatePositionFromEndPoints();
}

ImageObject::~ImageObject()
{}

QRectF ImageObject::boundingRect() const
{
    const qreal avgLat = (topLeft.latitude() + bottomRight.latitude()) / 2.0;
    const qreal lonPerMeter = Conversions::degreesLonPerMeter(avgLat);
    const qreal latPerMeter = Conversions::degreesLatPerMeter(avgLat);

    const qreal widthLon = qAbs<qreal>(topLeft.longitude() - bottomRight.longitude());
    const qreal heightlat = qAbs<qreal>(topLeft.latitude() - bottomRight.latitude());

    const qreal widthMeters = qMax<qreal>(widthLon / lonPerMeter, 5.0);
    const qreal heightMeters = qMax<qreal>(heightlat / latPerMeter, 5.0);

    const QRectF toRet(-1.0 * widthMeters,
                       -1.0 * heightMeters,
                       2.0 * widthMeters,
                       2.0 * heightMeters);

    return toRet;
}

void ImageObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget* w)
{
    Q_UNUSED(option)
    Q_UNUSED(w)

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);

    const qreal avgLat = (topLeft.latitude() + bottomRight.latitude()) / 2.0;
    const qreal lonPerMeter = Conversions::degreesLonPerMeter(avgLat);
    const qreal latPerMeter = Conversions::degreesLatPerMeter(avgLat);

    const QPointF center = this->pos();
    const QPointF offsetA = topLeft.lonLat() - center;
    const QPointF offsetB = bottomRight.lonLat() - center;

    const QPointF metersA(offsetA.x() / lonPerMeter,
                          offsetA.y() / latPerMeter);
    const QPointF metersB(offsetB.x() / lonPerMeter,
                          offsetB.y() / latPerMeter);
        //qDebug() << "drawimg";
    painter->drawImage(QRectF(metersA, metersB), img);
}

void ImageObject::updatePositionFromEndPoints()
{
    const qreal avgLon = (topLeft.longitude() + bottomRight.longitude()) / 2.0;
    const qreal avgLat = (topLeft.latitude() + bottomRight.latitude()) / 2.0;
    this->setPos(QPointF(avgLon, avgLat));
}

