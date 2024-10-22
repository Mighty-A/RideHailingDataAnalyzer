﻿#include "graphdialog.h"
#include "ui_graphdialog.h"

inline int LocatePointInGrid(QPointF p, QVector<QVector<qreal>>* grid) {
    const static qreal bottom = 30.524081949676;
    const static qreal top = 30.7938780503239;
    const static qreal left = 103.908407474531;
    const static qreal right = 104.222044525468;
    int indexX = floor((p.x() - left) / ((right - left) / 10));
    int indexY = floor((p.y() - bottom) / ((top - bottom) / 10));
    indexX = std::max(0, std::min(indexX, 9));
    indexY = std::max(0, std::min(indexY, 9));
    return indexX + 10 * indexY;
}

long long FromDateTimetoInt(QDateTime t) {
    return t.toMSecsSinceEpoch() / 1000;
}

QDateTime FromInttoDateTime(long long time) {
    QDateTime tmp;
    tmp.setMSecsSinceEpoch((time) * 1000);
    return tmp;
}

GraphDialog::GraphDialog(QWidget* qparent, MainWindow *parent) :
    QDialog(qparent),
    ui(new Ui::GraphDialog),
    mainwindow(parent)
{
    ui->setupUi(this);

    this->dataFrame = parent->dataFrame;
    this->grid = parent->grid;
    this->dataInGrid = parent->dataInGrid;
    this->startTime = parent->startTime;
    this->endTime = parent->endTime;
    this->gridLowerX = parent->gridLowerX;
    this->gridUpperX = parent->gridUpperX;
    this->gridLowerY = parent->gridLowerY;
    this->gridUpperY = parent->gridUpperY;

    // line chart
    lineChart = new QChart;
    lineChart->setAnimationOptions(QChart::SeriesAnimations);
    ui->graphicsView->setChart(lineChart);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setRubberBand(QChartView::HorizontalRubberBand);

    connect(ui->comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &GraphDialog::SetOrderChart);
    // init number of order chart
    ui->comboBox->setCurrentIndex(ChartTimeStep::thirtyMinutes);

    // bar Chart
    barChart = new QChart;
    barChart->setAnimationOptions(QChart::SeriesAnimations);
    ui->barGraphView->setChart(barChart);
    ui->barGraphView->setRenderHint(QPainter::Antialiasing);
    ui->barGraphView->setRubberBand(QChartView::HorizontalRubberBand);

    connect(ui->comboBox_2, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &GraphDialog::SetBarChart);
    ui->comboBox_2->setCurrentIndex(ChartTimeStep::thirtyMinutes);

    // scatter Chart
    scatterChart = new QChart;
    scatterChart->setAnimationOptions(QChart::SeriesAnimations);
    ui->scatterView->setChart(scatterChart);
    ui->scatterView->setRenderHint(QPainter::Antialiasing);
    ui->scatterView->setRubberBand(QChartView::RectangleRubberBand);

    scatterData = new QVector<QVector<FeeTime>>();
    connect(ui->comboBox_3, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &GraphDialog::SetScatterChart);
    ui->comboBox_3->setCurrentIndex(ChartTimeStep::thirtyMinutes);
    connect(ui->scatterSlider, &QSlider::sliderReleased, this, &GraphDialog::UpdateScatterChart);
    connect(ui->scatterSlider, &QSlider::valueChanged, this, &GraphDialog::UpdateScatterChartTimeLabel);
}

GraphDialog::~GraphDialog()
{
    delete ui;
    scatterData->clear();
    delete scatterData;
}

void GraphDialog::SetOrderChart(int timeStepIndex)
{
    //graphData.clear();
    lineChart->removeAllSeries();
    if (lineChart->axisX())
        lineChart->removeAxis(lineChart->axisX());
    if (lineChart->axisY())
        lineChart->removeAxis(lineChart->axisY());
    timeStep = timeStepValue[timeStepIndex];
    int* count = new int[(endTime - startTime) / timeStep + 10]{0};
    for (int ii = gridLowerX; ii <= gridUpperX; ii++) {
        for (int jj = gridLowerY; jj <= gridUpperY; jj++) {
            int gridIndex = ii + jj * 10;
            for (int t = 0; t < (*dataInGrid)[gridIndex][1].size(); t++) {
                const DataEntry* tmp = (*dataInGrid)[gridIndex][1][t];
                if (startTime <= tmp->departureTime && tmp->departureTime <= endTime) {
                    //graphData.push_back(tmp);
                    int timeIndex = (tmp->departureTime - startTime) / timeStep;
                    count[timeIndex]++;
                }
            }
            for (int t = 0; t < (*dataInGrid)[gridIndex][2].size(); t++) {
                const DataEntry* tmp = (*dataInGrid)[gridIndex][2][t];
                if (startTime <= tmp->departureTime && tmp->departureTime <= endTime) {
                    //graphData.push_back(tmp);
                    int timeIndex = (tmp->departureTime - startTime) / timeStep;
                    count[timeIndex]++;
                }
            }
        }
    }
    //bool compareByDepartureTime(const DataEntry* a, const DataEntry* b);
    //std::sort(graphData.begin(), graphData.end(), compareByDepartureTime);
    QSplineSeries* series = new QSplineSeries();
    series->useOpenGL();
    int tmpMax = 0;
    for (int i = 0; i < (endTime - startTime) / timeStep; i++) {
        series->append((timeStep * i + startTime) * 1000, count[i]);
        if (count[i] > tmpMax) {
            tmpMax = count[i];
        }
    }

    QFont axisFont("Arial", 7);
    // axisX
    QDateTimeAxis* axisX = new QDateTimeAxis;
    axisX->setTickCount(5);
    axisX->setFormat("dd h:00");
    axisX->setTitleText("time");
    axisX->setGridLineVisible(true);
    axisX->setLabelsAngle(-45);
    axisX->setLabelsFont(axisFont);



    axisX->setLineVisible(true);
    axisX->setRange(FromInttoDateTime(startTime), FromInttoDateTime(endTime));
    lineChart->addAxis(axisX, Qt::AlignBottom);

    // axisY
    QValueAxis* axisY = new QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTitleText("order number");
    axisY->setRange(0, tmpMax);
    axisY->setLabelsFont(axisFont);

    lineChart->addAxis(axisY, Qt::AlignLeft);
    lineChart->setTitle("number of order requests in grids selected");
    lineChart->addSeries(series);
    series->attachAxis(axisX);



}

void GraphDialog::SetBarChart(int timeStepIndex)
{
    barChart->removeAllSeries();
    if (barChart->axisX())
        barChart->removeAxis(barChart->axisX());
    if (barChart->axisY())
        barChart->removeAxis(barChart->axisY());
    timeStep = timeStepValue[timeStepIndex];
    int* countOutFlow = new int[(endTime - startTime) / timeStep + 10]{0};
    int* countInFlow = new int[(endTime - startTime) / timeStep + 10]{0};
    int* countInternal = new int[(endTime - startTime) / timeStep + 10]{0};
    for (int ii = gridLowerX; ii <= gridUpperX; ii++) {
        for (int jj = gridLowerY; jj <= gridUpperY; jj++) {
            int gridIndex = ii + jj * 10;
            // InFlow in small grid
            for (int t = 0; t < (*dataInGrid)[gridIndex][0].size(); t++) {
                const DataEntry* tmp = (*dataInGrid)[gridIndex][0][t];
                if (startTime <= tmp->departureTime && tmp->departureTime <= endTime) {
                    //graphData.push_back(tmp);
                    int timeIndex = (tmp->departureTime - startTime) / timeStep;
                    int orig = LocatePointInGrid(tmp->orig, grid);
                    int tmpi = orig % 10;
                    int tmpj = orig / 10;
                    if (tmpi >= gridLowerX && tmpi <= gridUpperX && tmpj >= gridLowerY && tmpj <= gridUpperY) {
                        countInternal[timeIndex]++;
                    } else {
                        countInFlow[timeIndex]++;
                    }
                }
            }
            // OutFlow in small grid
            for (int t = 0; t < (*dataInGrid)[gridIndex][1].size(); t++) {
                const DataEntry* tmp = (*dataInGrid)[gridIndex][1][t];
                if (startTime <= tmp->departureTime && tmp->departureTime <= endTime) {
                    //graphData.push_back(tmp);
                    int timeIndex = (tmp->departureTime - startTime) / timeStep;
                    int dest = LocatePointInGrid(tmp->dest, grid);
                    int tmpi = dest % 10;
                    int tmpj = dest / 10;
                    if (tmpi >= gridLowerX && tmpi <= gridUpperX && tmpj >= gridLowerY && tmpj <= gridUpperY) {
                       // do nothing to avoid dupli
                    } else {
                        countOutFlow[timeIndex]++;
                    }
                }
            }
            // Internal in small grid
            for (int t = 0; t < (*dataInGrid)[gridIndex][2].size(); t++) {
                const DataEntry* tmp = (*dataInGrid)[gridIndex][2][t];
                if (startTime <= tmp->departureTime && tmp->departureTime <= endTime) {
                    int timeIndex = (tmp->departureTime - startTime) / timeStep;
                    countInternal[timeIndex]++;
                }
            }
        }
    }
    QBarSet* inFlow, *outFlow, *internal;
    inFlow = new QBarSet("orders entering this area");
    outFlow = new QBarSet("orders leaving this area");
    internal = new QBarSet("orders inside this area");
    int tmpMax = 0;
    for (int i = 0; i < (endTime - startTime) / timeStep; i++) {
        *inFlow << countInFlow[i];
        *outFlow << countOutFlow[i];
        *internal << countInternal[i];
        if (countInFlow[i] + countOutFlow[i] + countInternal[i] > tmpMax) {
            tmpMax = countInFlow[i] + countOutFlow[i] + countInternal[i];
        }
    }

    QPercentBarSeries *series = new QPercentBarSeries();
    series->append(inFlow);
    series->append(outFlow);
    series->append(internal);

    barChart->addSeries(series);

    QFont axisFont("Arial", 7);
    // axisX
    QStringList categories;
    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    for (int i = 0; i < (endTime - startTime) / timeStep; i++) {
        categories.append(FromInttoDateTime((timeStep * i + startTime)).toString("dd h:00"));
    }
    axisX->append(categories);
    barChart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    axisX->setLabelsFont(axisFont);
    axisX->setLabelsAngle(-90);


    // axisY
    QValueAxis* axisY = new QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTitleText("order percentage");
    axisY->setRange(0, 100);
    axisY->setLabelsFont(axisFont);

    barChart->addAxis(axisY, Qt::AlignLeft);
    barChart->setTitle("orders entering/leaving/inside this area");
    barChart->legend()->setVisible(true);
    barChart->legend()->setAlignment(Qt::AlignBottom);
    barChart->legend()->setFont(axisFont);




}

void GraphDialog::SetScatterChart(int timeStepIndex)
{
    scatterData->clear();
    scatterChart->removeAllSeries();

    timeStep = timeStepValue[timeStepIndex];
    ui->scatterSlider->setRange(0, (endTime - startTime) / timeStep);
    for (int i = 0; i <= (endTime - startTime) / timeStep; i++) {
        scatterData->push_back(QVector<FeeTime>());
    }


    for (int ii = gridLowerX; ii <= gridUpperX; ii++) {
        for (int jj = gridLowerY; jj <= gridUpperY; jj++) {
            int gridIndex = ii + jj * 10;
            for (int t = 0; t < (*dataInGrid)[gridIndex][0].size(); t++) {
                const DataEntry* tmp = (*dataInGrid)[gridIndex][0][t];
                if (startTime <= tmp->departureTime && tmp->departureTime < endTime) {
                    int timeIndex = (tmp->departureTime - startTime) / timeStep;
                    FeeTime tmpft = {tmp->fee, tmp->endTime - tmp->departureTime};
                    (*scatterData)[timeIndex].push_back(tmpft);
                }
            }
        }
    }

    scatterChart->setTitle("fee-time scatter");
    ui->scatterSlider->setValue(0);
    UpdateScatterChart();
}

void GraphDialog::UpdateScatterChart() {
    int step = ui->scatterSlider->value();
    scatterChart->removeAllSeries();
    if (scatterChart->axisX())
        scatterChart->removeAxis(scatterChart->axisX());
    if (scatterChart->axisY())
        scatterChart->removeAxis(scatterChart->axisY());
    QScatterSeries* series = new QScatterSeries();
    series->setName("fee-time");
    series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    series->setMarkerSize(5.0);
    for (int i = 0; i < (*scatterData)[step].size(); i++) {
        qreal tmpTime = qreal((*scatterData)[step][i].time) / 60;
        qreal tmpFee = (*scatterData)[step][i].fee;
        *series << QPointF(tmpTime, tmpFee);
    }
    series->useOpenGL();
    scatterChart->addSeries(series);

    QFont axisFont("Arial", 7);
    // axisX
    QValueAxis* axisX = new QValueAxis;
    axisX->setTickCount(5);
    axisX->setTitleText("time/minutes");
    axisX->setGridLineVisible(true);
    axisX->setLabelsAngle(-45);
    axisX->setLabelsFont(axisFont);




    axisX->setLineVisible(true);
    axisX->setRange(0, 160);
    scatterChart->addAxis(axisX, Qt::AlignBottom);

    // axisY
    QValueAxis* axisY = new QValueAxis;
    axisY->setTitleText("fee/yuan");
    axisY->setLabelsFont(axisFont);
    axisY->setRange(0, 50);


    scatterChart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

}

void GraphDialog::UpdateScatterChartTimeLabel(int t)
{
    QDateTime currentTime = FromInttoDateTime(startTime + timeStep * t);
    ui->timeLabel->setText(currentTime.toString());
}

void GraphDialog::on_FeeTimeButtonRight_clicked()
{
    int step = ui->scatterSlider->value();
    if (step == (endTime - startTime) / timeStep) {
        return;
    }
    ui->scatterSlider->setValue(step + 1);
    UpdateScatterChart();
}



void GraphDialog::on_FeeTimeButtonLeft_clicked()
{
    int step = ui->scatterSlider->value();
    if (step == 0) {
        return;
    }
    ui->scatterSlider->setValue(step - 1);
    UpdateScatterChart();
}
