#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "DataEntry.h"
#include "rangeslider.h"
#include "MapGraphicsScene.h"
#include "MapGraphicsObject.h"
#include "MapGraphicsView.h"
#include "tileSources/OSMTileSource.h"
#include "tileSources/CompositeTileSource.h"
#include "tileSources/mytilesource.h"
#include "tileSources/GridTileSource.h"
#include "LineObject.h"
#include "graphdialog.h"
#include "CircleObject.h"
#include "PointObject.h"
#include "visualizationdialog.h"

#include <QMainWindow>
#include <QMessageBox>
#include <iostream>
#include <QThread>
#include <QGroupBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

long long FromDateTimetoInt(QDateTime t);
QDateTime FromInttoDateTime(long long time);
int LocatePointInGrid(QPointF p, QVector<QVector<qreal>>* grid);

class Worker : public QObject
{
    Q_OBJECT
public:
    enum dataInGridType {INFLOW, OUTFLOW, INTERNAL};
    Worker(QObject* parent = nullptr);
    ~Worker();
signals:
    void sig_finish();

public slots:
    void slt_dowork(
            QVector<DataEntry>* dataFrame,
            QVector<QVector<qreal>>* grid,
            QVector<QVector<QVector<const DataEntry*>>>* dataInGrid
    );

};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void ReceiveShow(QVector<DataEntry>* dataFrame, QVector<QVector<qreal>>* grid);
    void SetRect(QPointF bottomLeft, QPointF topRight);
    void PreprocessFinished();
private:
    Ui::MainWindow *ui;
    QMutex mutex;
public:
    QVector<DataEntry>* dataFrame = nullptr;
    QVector<QVector<qreal>>* grid = nullptr;

    QVector<QVector<QVector<const DataEntry*>>>* dataInGrid;

    RangeSlider* timeSpanSlider;
    RangeSlider* fieldsLngSlider;
    RangeSlider* fieldsLatSlider;
    long long lowerTimeBound;
    long long upperTimeBound;
    long long startTime;
    long long endTime;
    QGroupBox *mapBox;
    int gridLowerX = 0;
    int gridUpperX = 9;
    int gridLowerY = 0;
    int gridUpperY = 9;
    MapGraphicsView * view;
    MapGraphicsScene * scene;
    QSharedPointer<CompositeTileSource> composite;
    LineObject * Rect[4];
    Worker* preprocess = nullptr;
    QThread* pthread = nullptr;



private slots:
    void SetStartTimeFromEdit(const QDateTime &tmpDateTime);
    void SetEndTimeFromEdit(const QDateTime &tmpDateTime);
    void SetTimeFromSlider(int aLowerTime, int aUpperTime);
    void SetFieldsFromHorizontalSliders(int lower, int upper);
    void SetFieldsFromVerticalSliders(int lower, int upper);

    void on_graphButton_clicked();

    void on_visualButton_clicked();

signals:
    void UpdateMap();
    void Preprocess(
            QVector<DataEntry>* dataFrame,
            QVector<QVector<qreal>>* grid,
            QVector<QVector<QVector<const DataEntry*>>>* dataInGrid
    );
};

#endif // MAINWINDOW_H

