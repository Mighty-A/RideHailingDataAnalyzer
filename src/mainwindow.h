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

#include <QMainWindow>
#include <QMessageBox>
#include <iostream>
#include <QGroupBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void ReceiveShow(QVector<DataEntry>* dataFrame, QVector<QVector<qreal>>* grid);
    void SetRect(QPointF bottomLeft, QPointF topRight);

private:
    Ui::MainWindow *ui;
    QVector<DataEntry>* dataFrame = nullptr;
    QVector<QVector<qreal>>* grid = nullptr;
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


private slots:
    void SetStartTimeFromEdit(const QDateTime &tmpDateTime);
    void SetEndTimeFromEdit(const QDateTime &tmpDateTime);
    void SetTimeFromSlider(int aLowerTime, int aUpperTime);
    void SetFieldsFromHorizontalSliders(int lower, int upper);
    void SetFieldsFromVerticalSliders(int lower, int upper);

signals:
    void UpdateMap();
};

#endif // MAINWINDOW_H

