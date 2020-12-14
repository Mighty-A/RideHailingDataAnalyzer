#ifndef VISUALIZATIONDIALOG_H
#define VISUALIZATIONDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QColor>
#include "MapGraphicsObject.h"
#include "MapGraphicsScene.h"
#include "MapGraphicsView.h"
#include "mainwindow.h"
#include "DataEntry.h"
#include "ImageObject.h"


namespace Ui {
class VisualizationDialog;
}

class MainWindow;

class VisualizationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VisualizationDialog(QWidget *qparent = nullptr, MainWindow* parent = nullptr);
    ~VisualizationDialog();

    enum ChartTimeStep { tenMinutes, thirtyMinutes, oneHour, twoHour, sixHour, twelveHour, oneDay};
    const int timeStepValue[7] = {600, 1800, 3600, 7200, 21600, 43200, 86400};

private:
    Ui::VisualizationDialog *ui;

    MainWindow* mainwindow;
    // data from main
    QVector<DataEntry>* dataFrame = nullptr;
    QVector<QVector<qreal>>* grid = nullptr;
    QVector<QVector<QVector<const DataEntry*>>>* dataInGrid;
    QVector<const DataEntry*> graphData;
    long long startTime;
    long long endTime;
    int gridLowerX = 0;
    int gridUpperX = 9;
    int gridLowerY = 0;
    int gridUpperY = 9;
    int timeStep;
    // heat map v1
    LineObject* Rect[4];
    MapGraphicsView * view;
    MapGraphicsScene * scene;
    QSharedPointer<CompositeTileSource> composite;
    const qreal pointSize =  0.000313 * 8;
    const qreal pointOpacity = 0.4;

    QVector<QVector<const DataEntry*>>* heatMapData;
    QSlider* heatMapSlider;

    // heat map v2
    LineObject* RectV2[4];
    MapGraphicsView * viewV2;
    MapGraphicsScene * sceneV2;
    QSharedPointer<CompositeTileSource> compositeV2;

    QVector<QVector<const DataEntry*>>* heatMapDataV2;
    QSlider* heatMapSliderV2;
    int timeStepV2;
    QImage *dataImg, *heatImg;
    const qreal pointSizeV2 = 8;
    QRgb colorList[256];

public slots:
    void SetHeatMap(int timeStep);
    void UpdateHeatMap();

    void SetHeatMapV2(int timeStep);
    void UpdateHeatMapV2();

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_heatMapV2ButtonRight_clicked();
    void on_heatMapV2ButtonLeft_clicked();
};

#endif // VISUALIZATIONDIALOG_H
