#include "visualizationdialog.h"
#include "ui_visualizationdialog.h"

VisualizationDialog::VisualizationDialog(QWidget *qparent, MainWindow *parent) :
    QDialog(qparent),
    mainwindow(parent),
    ui(new Ui::VisualizationDialog)
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

    // Heat Map
    scene = new MapGraphicsScene(this);
    view = new MapGraphicsView(scene, this);
    ui->mapLayout->addWidget(view);

    QSharedPointer<OSMTileSource> osmTiles(new OSMTileSource(OSMTileSource::OSMTiles), &QObject::deleteLater);
    QSharedPointer<MyTileSource> rectTiles(new MyTileSource(), &QObject::deleteLater);
    QSharedPointer<CompositeTileSource> composite(new CompositeTileSource(), &QObject::deleteLater);
    composite->addSourceBottom(osmTiles);
    composite->addSourceTop(rectTiles);
    view->setTileSource(composite);

    heatMapData = new QVector<QVector<const DataEntry*>>;
    heatMapData->push_back(QVector<const DataEntry*>());

    heatMapSlider = new QSlider(Qt::Horizontal, this);
    ui->mapLayout->addWidget(heatMapSlider);
    // map init
    view->setZoomLevel(11);
    view->centerOn(104.0652259999995, 30.65897999999995);

    QPointF bottomLeft = QPointF((*grid)[gridLowerX + gridLowerY * 10][6], (*grid)[gridLowerX + gridLowerY * 10][7]);
    QPointF topRight = QPointF((*grid)[gridUpperX + gridUpperY * 10][2], (*grid)[gridUpperX + gridUpperY * 10][3]);

    for (int i = 0; i < 4; i++) {
        Rect[i] = new LineObject(Position(0, 0), Position(0, 0));
        scene->addObject(Rect[i]);
    }
    Rect[0]->setEndPointA(Position(bottomLeft.x(), bottomLeft.y()));
    Rect[0]->setEndPointB(Position(topRight.x(), bottomLeft.y()));
    Rect[1]->setEndPointA(Position(topRight.x(), bottomLeft.y()));
    Rect[1]->setEndPointB(Position(topRight.x(), topRight.y()));
    Rect[2]->setEndPointA(Position(topRight.x(), topRight.y()));
    Rect[2]->setEndPointB(Position(bottomLeft.x(), topRight.y()));
    Rect[3]->setEndPointA(Position(bottomLeft.x(), topRight.y()));
    Rect[3]->setEndPointB(Position(bottomLeft.x(), bottomLeft.y()));

    connect(ui->comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &VisualizationDialog::SetHeatMap);
    ui->comboBox->setCurrentIndex(ChartTimeStep::thirtyMinutes);
    connect(heatMapSlider, &QSlider::sliderReleased, this, &VisualizationDialog::UpdateHeatMap);
}

VisualizationDialog::~VisualizationDialog()
{
    delete ui;
}

void VisualizationDialog::SetHeatMap(int timeStepIndex)
{

    heatMapData->clear();
    timeStep = timeStepValue[timeStepIndex];
    heatMapSlider->setRange(0, (endTime - startTime) / timeStep);
    for (int i = 0; i  <= (endTime - startTime) / timeStep; i++) {
        heatMapData->push_back(QVector<const DataEntry*>());
    }

    for (int ii = gridLowerX; ii <= gridUpperX; ii++) {
        for (int jj = gridLowerY; jj <= gridUpperY; jj++) {
            int gridIndex = ii + jj * 10;
            for (int t = 0; t < (*dataInGrid)[gridIndex][1].size(); t++) {
                const DataEntry* tmp = (*dataInGrid)[gridIndex][1][t];
                if (startTime <= tmp->departureTime && tmp->departureTime <= endTime) {
                    int timeIndex = (tmp->departureTime - startTime) / timeStep;
                    (*heatMapData)[timeIndex].push_back(tmp);
                }
            }
            for (int t = 0; t < (*dataInGrid)[gridIndex][2].size(); t++) {
                const DataEntry* tmp = (*dataInGrid)[gridIndex][2][t];
                if (startTime <= tmp->departureTime && tmp->departureTime <= endTime) {
                    int timeIndex = (tmp->departureTime - startTime) / timeStep;
                    (*heatMapData)[timeIndex].push_back(tmp);
                }
            }
        }
    }
    heatMapSlider->setValue(0);
    UpdateHeatMap();
}

void VisualizationDialog::UpdateHeatMap()
{
    int step = heatMapSlider->value();
    QDateTime currentTime = FromInttoDateTime(startTime + timeStep * step);
    ui->heatMapLabel->setText(currentTime.toString());
    QList<MapGraphicsObject*> toBeRemoved = scene->objects();
    qDebug() << toBeRemoved.size();
    for (int i = 0; i < toBeRemoved.size(); i++) {
        scene->removeObject(toBeRemoved[i]);
        //delete toBeRemoved[i];
    }

    // line
    QPointF bottomLeft = QPointF((*grid)[gridLowerX + gridLowerY * 10][6], (*grid)[gridLowerX + gridLowerY * 10][7]);
    QPointF topRight = QPointF((*grid)[gridUpperX + gridUpperY * 10][2], (*grid)[gridUpperX + gridUpperY * 10][3]);


    for (int i = 0; i < 4; i++) {
        Rect[i] = new LineObject(Position(0, 0), Position(0, 0));
        scene->addObject(Rect[i]);
    }
    Rect[0]->setEndPointA(Position(bottomLeft.x(), bottomLeft.y()));
    Rect[0]->setEndPointB(Position(topRight.x(), bottomLeft.y()));
    Rect[1]->setEndPointA(Position(topRight.x(), bottomLeft.y()));
    Rect[1]->setEndPointB(Position(topRight.x(), topRight.y()));
    Rect[2]->setEndPointA(Position(topRight.x(), topRight.y()));
    Rect[2]->setEndPointB(Position(bottomLeft.x(), topRight.y()));
    Rect[3]->setEndPointA(Position(bottomLeft.x(), topRight.y()));
    Rect[3]->setEndPointB(Position(bottomLeft.x(), bottomLeft.y()));

    qDebug() << (*heatMapData)[step].size();
    // point

    for (int i = 0; i < (*heatMapData)[step].size(); i++) {
        /*
        if (i % 100 == 0) {
            qDebug() << (*heatMapData)[step][i]->orig.x();
        }*/
        scene->addObject(new PointObject(
                             Position((*heatMapData)[step][i]->orig),
                             pointSize,
                             pointOpacity,
                             QColor(0, 0, 255, 100)
                             ));
    }

    //qDebug() << '1';

}


void VisualizationDialog::on_pushButton_2_clicked()
{
    int step = heatMapSlider->value();
    if (step == 0) {
        return;
    }
    heatMapSlider->setValue(step - 1);
    UpdateHeatMap();
}

void VisualizationDialog::on_pushButton_clicked()
{
    int step = heatMapSlider->value();
    if (step == (endTime - startTime) / timeStep) {
        return;
    }
    heatMapSlider->setValue(step + 1);
    UpdateHeatMap();
}
