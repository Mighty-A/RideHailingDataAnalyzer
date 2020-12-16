#ifndef PREDICTIONDIALOG_H
#define PREDICTIONDIALOG_H

#include <QDebug>
#include <QColor>
#include "MapGraphicsObject.h"
#include "MapGraphicsScene.h"
#include "MapGraphicsView.h"
#include "mainwindow.h"
#include "DataEntry.h"
#include "ImageObject.h"
#include "networkmanager.h"
#include "predictmodel.h"

#include <QDialog>

namespace Ui {
class PredictionDialog;
}

class PredictionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PredictionDialog(QWidget *qparent = nullptr, MainWindow* parent = nullptr);
    ~PredictionDialog();

private:
    enum Weekday {Sunday, Monday, TuesDay, Wedesday, Thursday, Friday, Saturday};
    Ui::PredictionDialog *ui;
    MainWindow* mainwindow;
    QVector<DataEntry>* dataFrame = nullptr;
    QVector<QVector<qreal>>* grid = nullptr;
    QVector<QVector<QVector<const DataEntry*>>>* dataInGrid;
    NetworkManager *manager;

    MapGraphicsView *view;
    MapGraphicsScene *scene;
    QSharedPointer<CompositeTileSource> composite;
    int currentWeekday = 0;
    int currentTime = 0;


    QCompleter* completer;
    QStringList tips;
    QStringListModel* stringListModelForCompleter;
    QVector<QPointF> locations;
    QPointF orig = QPointF(0, 0);
    QPointF dest = QPointF(0, 0);
    PointObject* origPoint;
    PointObject* destPoint;
    ImageObject* origIcon;
    ImageObject* destIcon;
    QString origText;
    QString destText;

    const qreal latlonOffset = 0.03;  // about 100 meters
    const int timeOffset = 30;
    const qreal icon_size = 0.009;
    PredictModel *model;


    qreal dist(const QPointF& a, const QPointF& b);
private slots:
    void SetWeekday(int);
    void SendOrigTipsRequest(QString);
    void SendDestTipsRequest(QString);
    void ReceiveTips(QStringList, QVector<QPointF>);
    void SetTime(QTime time);
    void on_OrigButton_clicked();
    void on_DestButton_clicked();
    void on_predictButton_clicked();
    void UpdateMap();
};

#endif // PREDICTIONDIALOG_H
