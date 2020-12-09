#include "PointObject.h"
#include "guts/Conversions.h"
#include <QtGlobal>

PointObject::PointObject(const Position& p,
                         qreal size,
                         qreal thickness,
                         QColor color,
                         MapGraphicsObject* parent
                         ) :
    MapGraphicsObject(false, parent),
    _p(p),
    _brushColor(color)
{
    _thickness =  qBound<qreal>(0.0, thickness, 1.0);
    _size = qBound<qreal>(0.0, size, 2);
    this->updatePositionFromPoint();
}

PointObject::~PointObject()
{

}

QRectF PointObject::boundingRect() const
{
    const qreal avglat = _p.latitude();
    const qreal lonPerMeter = Conversions::degreesLonPerMeter(_p.latitude());
    const qreal latPerMeter = Conversions::degreesLatPerMeter(_p.latitude());
    qreal widthLon = 2 * _size / lonPerMeter;
    qreal heightlat = 2 * _size / lonPerMeter;


    const QRectF toRet(-1.0 * widthLon,
                       -1.0 * heightlat,
                       2.0 * widthLon,
                       2.0 * heightlat);
    return toRet;
}

void PointObject::paint(QPainter *painter,
                        const QStyleOptionGraphicsItem *option,
                        QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    //qDebug() << "painting point";
    //qDebug() << pos().x() << ' ' << pos().y();
    painter->setRenderHint(QPainter::Antialiasing, true);
    QPen pen = painter->pen();
    pen.setWidthF(10.0);
    painter->setPen(Qt::NoPen);

    painter->setBrush(brushColor());
    painter->setOpacity(_thickness);

    const qreal avglat = _p.latitude();
    const qreal lonPerMeter = Conversions::degreesLonPerMeter(_p.latitude());
    const qreal latPerMeter = Conversions::degreesLatPerMeter(_p.latitude());

    const QPointF center = this->pos();
    const QPointF topLeft = QPointF(_p.longitude() / lonPerMeter -  _size / lonPerMeter - center.x() / lonPerMeter,
                                    _p.latitude() / latPerMeter - _size / latPerMeter - center.y() / latPerMeter);

    //qDebug() << topLeft.x() << ' ' << topLeft.y();
    painter->drawEllipse(QPointF(0, 0), _size / lonPerMeter, _size / lonPerMeter);
}

qreal PointObject::thickness() const {
    return _thickness;
}

void PointObject::setThickness(qreal nThick) {
    _thickness = qBound<qreal>(0.0, nThick, 1.0);
    this->redrawRequested();
}

void PointObject::setPoint(const Position& p)
{
    _p = p;
    this->updatePositionFromPoint();
    this->redrawRequested();
}

void PointObject::updatePositionFromPoint()
{
    const qreal avgLon = _p.longitude();
    const qreal avgLat = _p.latitude();
    this->setPos(QPointF(avgLon, avgLat));
}

QColor PointObject::brushColor() const
{
    return _brushColor;
}

void PointObject::setBrushColor(QColor mColor)
{
    _brushColor = mColor;
    this->redrawRequested();
}
