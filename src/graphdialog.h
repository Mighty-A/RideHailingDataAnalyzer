#ifndef GRAPHDIALOG_H
#define GRAPHDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QtCharts>
#include "DataEntry.h"
#include "mainwindow.h"
#include <algorithm>
#include <QDebug>
#include <QSplineSeries>

namespace Ui {
class GraphDialog;
}

class MainWindow;
class GraphDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GraphDialog(QWidget* qparent, MainWindow* parent);
    ~GraphDialog();

private:
    Ui::GraphDialog *ui;
    enum ChartTimeStep { tenMinutes, thirtyMinutes, oneHour, twoHour, sixHour, twelveHour, oneDay};
    const int timeStepValue[7] = {600, 1800, 3600, 7200, 21600, 43200, 86400};
    QChart *lineChart;
    QChart *barChart;
    QChart *scatterChart;
    struct FeeTime {
        qreal fee;
        long long time;
    };

    QVector<QVector<FeeTime>> *scatterData;

private:
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

public slots:
    void SetOrderChart(int timeStep);
    void SetBarChart(int timeStep);
    void SetScatterChart(int timeStep);
    void UpdateScatterChart();
};

#endif // GRAPHDIALOG_H
