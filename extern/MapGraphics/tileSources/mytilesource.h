#ifndef MYTILESOURCE_H
#define MYTILESOURCE_H


#include "MapTileSource.h"

#include "MapGraphics_global.h"

class MAPGRAPHICSSHARED_EXPORT MyTileSource : public MapTileSource
{
    Q_OBJECT
public:
    explicit MyTileSource();
    virtual ~MyTileSource();

    QPointF bottomLeft = QPoint(0, 0);
    QPointF topRight = QPoint(0, 0);

    virtual QPointF ll2qgs(const QPointF& ll, quint8 zoomLevel) const;

    virtual QPointF qgs2ll(const QPointF& qgs, quint8 zoomLevel) const;

    virtual quint64 tilesOnZoomLevel(quint8 zoomLevel) const;

    virtual quint16 tileSize() const;

    virtual quint8 minZoomLevel(QPointF ll);

    virtual quint8 maxZoomLevel(QPointF ll);

    virtual QString name() const;

    virtual QString tileFileExtension() const;

protected:
    virtual void fetchTile(quint32 x,
                           quint32 y,
                           quint8 z);

signals:
    void rectChanged();
public slots:
    void SetRect(QPointF, QPointF);
};
#endif // MYTILESOURCE_H
