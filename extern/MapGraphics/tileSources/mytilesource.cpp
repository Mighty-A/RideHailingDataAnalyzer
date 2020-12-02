#include "mytilesource.h"

#include <cmath>
#include <QPainter>
#include <QStringBuilder>
#include <QtDebug>

const qreal PI = 3.14159265358979323846;
const qreal deg2rad = PI / 180.0;
const qreal rad2deg = 180.0 / PI;

MyTileSource::MyTileSource() :
    MapTileSource()
{
    this->setCacheMode(MapTileSource::NoCaching);
}

MyTileSource::~MyTileSource()
{
    qDebug() << this << "destructing";
}

QPointF MyTileSource::ll2qgs(const QPointF &ll, quint8 zoomLevel) const
{
    const qreal tilesOnOneEdge = pow(2.0,zoomLevel);
    const quint16 tileSize = this->tileSize();
    qreal x = (ll.x()+180.0) * (tilesOnOneEdge*tileSize)/360.0; // coord to pixel!
    qreal y = (1-(log(tan(PI/4.0+(ll.y()*deg2rad)/2)) /PI)) /2.0  * (tilesOnOneEdge*tileSize);

    return QPoint(int(x), int(y));
}

QPointF MyTileSource::qgs2ll(const QPointF &qgs, quint8 zoomLevel) const
{
    const qreal tilesOnOneEdge = pow(2.0,zoomLevel);
    const quint16 tileSize = this->tileSize();
    qreal longitude = (qgs.x()*(360.0/(tilesOnOneEdge*tileSize)))-180.0;
    qreal latitude = rad2deg*(atan(sinh((1.0-qgs.y()*(2.0/(tilesOnOneEdge*tileSize)))*PI)));

    return QPointF(longitude, latitude);
}

quint64 MyTileSource::tilesOnZoomLevel(quint8 zoomLevel) const
{
    return pow(4.0,zoomLevel);
}

quint16 MyTileSource::tileSize() const
{
    return 256;
}

quint8 MyTileSource::minZoomLevel(QPointF ll)
{
    Q_UNUSED(ll)
    return 0;
}

quint8 MyTileSource::maxZoomLevel(QPointF ll)
{
    Q_UNUSED(ll)
    return 50;
}

QString MyTileSource::name() const
{
    return "My Tiles";
}

QString MyTileSource::tileFileExtension() const
{
    return "png";
}

void MyTileSource::fetchTile(quint32 x,
                       quint32 y,
                       quint8 z)
{
    quint64 leftScenePixel = x*this->tileSize();
    quint64 topScenePixel = y*this->tileSize();
    quint64 rightScenePixel = leftScenePixel + this->tileSize();
    quint64 bottomScenePixel = topScenePixel + this->tileSize();

    QImage * toRet = new QImage(this->tileSize(),
                                this->tileSize(),
                                QImage::Format_ARGB32_Premultiplied);
    //It is important to fill with transparent first!
    toRet->fill(qRgba(0,0,0,0));

    QPainter painter(toRet);
    QPen pen;
    pen.setColor(QColor(255, 0, 0));
    pen.setWidth(5);
    painter.setPen(pen);
    //painter.fillRect(toRet->rect(),QColor(0,0,0,0));

    // draw a rect
    QPointF p1 = this->ll2qgs(bottomLeft, z);
    QPointF p2 = this->ll2qgs(topRight, z);
    if (fabs(p1.x() - p2.x()) > 1e-7 && fabs(p1.y() - p2.y()) > 1e-7) {

        //if (p1.x() < rightScenePixel && p2.x() > leftScenePixel && p1.y() > topScenePixel && p1.y() < bottomScenePixel) {
            painter.drawLine(fmax(p1.x() - leftScenePixel, 0), p1.y() - topScenePixel, fmin(p2.x() - leftScenePixel, this->tileSize()), p1.y() - topScenePixel);
        //}
        //if (p1.x() < rightScenePixel && p2.x() > leftScenePixel && p2.y() > topScenePixel && p2.y() < bottomScenePixel) {
            painter.drawLine(fmax(p1.x() - leftScenePixel, 0), p2.y() - topScenePixel, fmin(p2.x() - leftScenePixel, this->tileSize()), p2.y() - topScenePixel);
        //}
        //if (p1.y() > topScenePixel && p2.y() < bottomScenePixel && p1.x() > leftScenePixel && p1.x() < rightScenePixel) {
            painter.drawLine(p1.x() - leftScenePixel, fmin(p1.y() - topScenePixel, this->tileSize()), p1.x() - leftScenePixel, fmax(p2.y() - topScenePixel, 0));
        //}
        //if (p1.y() > topScenePixel && p2.y() < bottomScenePixel && p2.x() > leftScenePixel && p2.x() < rightScenePixel) {
            painter.drawLine(p2.x() - leftScenePixel, fmin(p1.y() - topScenePixel, this->tileSize()), p2.x() - leftScenePixel, fmax(p2.y() - topScenePixel, 0));
        //}
    }
    painter.end();
    this->prepareNewlyReceivedTile(x,y,z,toRet);
}


void MyTileSource::SetRect(QPointF bl, QPointF tr) {
    bottomLeft = bl;
    topRight = tr;

    //emit allTilesInvalidated();
    //emit rectChanged();
}
