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

    // Heat Map V1
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
    //qDebug() << '1';
    // Heat Map V2

    sceneV2 = new MapGraphicsScene(this);
    viewV2 = new MapGraphicsView(sceneV2, this);
    ui->mapLayout_2->addWidget(viewV2);

    QSharedPointer<OSMTileSource> osmTilesV2(new OSMTileSource(OSMTileSource::OSMTiles), &QObject::deleteLater);
    QSharedPointer<MyTileSource> rectTilesV2(new MyTileSource(), &QObject::deleteLater);
    QSharedPointer<CompositeTileSource> compositeV2(new CompositeTileSource(), &QObject::deleteLater);

    compositeV2->addSourceBottom(osmTilesV2);
    compositeV2->addSourceTop(rectTilesV2);
    viewV2->setTileSource(compositeV2);

    heatMapDataV2 = new QVector<QVector<const DataEntry*>>;
    heatMapDataV2->push_back(QVector<const DataEntry*>());

    heatMapSliderV2 = new QSlider(Qt::Horizontal, this);
    ui->mapLayout_2->addWidget(heatMapSliderV2);

    viewV2->setZoomLevel(11);
    viewV2->centerOn(104.0652259999995, 30.65897999999995);

    for (int i = 0; i < 4; i++) {
        RectV2[i] = new LineObject(Position(0, 0), Position(0, 0));
        sceneV2->addObject(RectV2[i]);
    }
    RectV2[0]->setEndPointA(Position(bottomLeft.x(), bottomLeft.y()));
    RectV2[0]->setEndPointB(Position(topRight.x(), bottomLeft.y()));
    RectV2[1]->setEndPointA(Position(topRight.x(), bottomLeft.y()));
    RectV2[1]->setEndPointB(Position(topRight.x(), topRight.y()));
    RectV2[2]->setEndPointA(Position(topRight.x(), topRight.y()));
    RectV2[2]->setEndPointB(Position(bottomLeft.x(), topRight.y()));
    RectV2[3]->setEndPointA(Position(bottomLeft.x(), topRight.y()));
    RectV2[3]->setEndPointB(Position(bottomLeft.x(), bottomLeft.y()));

    colorList[256];

    QLinearGradient linear=QLinearGradient(QPoint(0,0),QPoint(255,0));
    linear.setColorAt(0, Qt::blue);
    linear.setColorAt(0.4, Qt::blue);
    linear.setColorAt(0.5, Qt::cyan);
    linear.setColorAt(0.6, Qt::green);
    linear.setColorAt(0.8, Qt::yellow);
    linear.setColorAt(0.95, Qt::red);

    //把渐变色绘制到Img方便取颜色
    QImage img(256,1,QImage::Format_ARGB32);
    QPainter painter(&img);
    painter.fillRect(img.rect(),linear);

    //HeatAlpha为热力图整体透明度
    quint32 alpha=0;

    for(quint32 i=0;i<256;i++){
        alpha=200/255.0*i;
        colorList[i]=img.pixel(i,0)&0x00FFFFFF|(alpha<<24);
    }

    connect(ui->comboBox_2, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &VisualizationDialog::SetHeatMapV2);
    ui->comboBox_2->setCurrentIndex(ChartTimeStep::tenMinutes);
    SetHeatMapV2(0);
    connect(heatMapSliderV2, &QSlider::sliderReleased, this, &VisualizationDialog::UpdateHeatMapV2);
}

VisualizationDialog::~VisualizationDialog()
{
    delete ui;
    heatMapData->clear();
    delete heatMapData;
    heatMapDataV2->clear();
    delete heatMapDataV2;
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
    //qDebug() << toBeRemoved.size();
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

    //qDebug() << (*heatMapData)[step].size();
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

void VisualizationDialog::SetHeatMapV2(int timeStepIndex)
{
    //qDebug() << '1';
    heatMapDataV2->clear();
    timeStepV2 = timeStepValue[timeStepIndex];
    heatMapSliderV2->setRange(0, (endTime - startTime) / timeStepV2);
    for (int i = 0; i  <= (endTime - startTime) / timeStepV2; i++) {
        heatMapDataV2->push_back(QVector<const DataEntry*>());
    }

    for (int ii = gridLowerX; ii <= gridUpperX; ii++) {
        for (int jj = gridLowerY; jj <= gridUpperY; jj++) {
            int gridIndex = ii + jj * 10;
            for (int t = 0; t < (*dataInGrid)[gridIndex][1].size(); t++) {
                const DataEntry* tmp = (*dataInGrid)[gridIndex][1][t];
                if (startTime <= tmp->departureTime && tmp->departureTime <= endTime) {
                    int timeIndex = (tmp->departureTime - startTime) / timeStepV2;
                    (*heatMapDataV2)[timeIndex].push_back(tmp);
                }
            }
            for (int t = 0; t < (*dataInGrid)[gridIndex][2].size(); t++) {
                const DataEntry* tmp = (*dataInGrid)[gridIndex][2][t];
                if (startTime <= tmp->departureTime && tmp->departureTime <= endTime) {
                    int timeIndex = (tmp->departureTime - startTime) / timeStepV2;
                    (*heatMapDataV2)[timeIndex].push_back(tmp);
                }
            }
        }
    }
    heatMapSliderV2->setValue(0);
    UpdateHeatMapV2();
}

void VisualizationDialog::UpdateHeatMapV2()
{
    int step = heatMapSliderV2->value();
    QDateTime currentTime = FromInttoDateTime(startTime + timeStepV2 * step);
    ui->heatMapLabel_2->setText(currentTime.toString());

    const int HeatAlpha = 200;
    QList<MapGraphicsObject*> toBeRemoved = sceneV2->objects();
    //qDebug() << toBeRemoved.size();
    for (int i = 0; i < toBeRemoved.size(); i++) {
        sceneV2->removeObject(toBeRemoved[i]);
        //delete toBeRemoved[i];
    }

    // line
    QPointF bottomLeft = QPointF((*grid)[gridLowerX + gridLowerY * 10][6], (*grid)[gridLowerX + gridLowerY * 10][7]);
    QPointF topRight = QPointF((*grid)[gridUpperX + gridUpperY * 10][2], (*grid)[gridUpperX + gridUpperY * 10][3]);


    for (int i = 0; i < 4; i++) {
        RectV2[i] = new LineObject(Position(0, 0), Position(0, 0));
        sceneV2->addObject(Rect[i]);
    }
    RectV2[0]->setEndPointA(Position(bottomLeft.x(), bottomLeft.y()));
    RectV2[0]->setEndPointB(Position(topRight.x(), bottomLeft.y()));
    RectV2[1]->setEndPointA(Position(topRight.x(), bottomLeft.y()));
    RectV2[1]->setEndPointB(Position(topRight.x(), topRight.y()));
    RectV2[2]->setEndPointA(Position(topRight.x(), topRight.y()));
    RectV2[2]->setEndPointB(Position(bottomLeft.x(), topRight.y()));
    RectV2[3]->setEndPointA(Position(bottomLeft.x(), topRight.y()));
    RectV2[3]->setEndPointB(Position(bottomLeft.x(), bottomLeft.y()));

    //qDebug() << (*heatMapDataV2)[step].size();


    // draw heatmap
    int pixelPerGrid = 100;
    const int imgHeight = pixelPerGrid * (gridUpperY - gridLowerY + 1);
    const int imgWidth = pixelPerGrid * (gridUpperX - gridLowerX + 1);

    int *countOfPoint = new int[(imgWidth + 1) * (imgHeight + 1)]{0};
    dataImg = new QImage(imgWidth, imgHeight, QImage::Format_Alpha8);
    dataImg->fill(Qt::transparent);
    heatImg = new QImage(imgWidth, imgHeight, QImage::Format_ARGB32);
    heatImg->fill(Qt::transparent);


    const static qreal bottom = 30.524081949676;
    const static qreal top = 30.7938780503239;
    const static qreal left = 103.908407474531;
    const static qreal right = 104.222044525468;
    qreal latPerGrid = (top - bottom) / 10;
    qreal lonPerGrid = (right - left) / 10;


    for (int i = 0; i < (*heatMapDataV2)[step].size(); i++) {
        int imgIndexX = floor((((*heatMapDataV2)[step][i]->orig.x() - (left + gridLowerX * lonPerGrid)) / lonPerGrid) * pixelPerGrid) ;
        int imgIndexY = floor((((*heatMapDataV2)[step][i]->orig.y() - (bottom + gridLowerY * latPerGrid)) / latPerGrid) * pixelPerGrid) ;
        //qDebug() << (*heatMapDataV2)[step][i]->orig.x();

        if (imgIndexX < 0 || imgIndexX >= imgWidth || imgIndexY < 0 || imgIndexY >= imgHeight) {
            continue;
        }
        //qDebug() << imgIndexX << ' ' << imgIndexY;
        countOfPoint[imgIndexX + imgIndexY * imgWidth]++;
    }

    int countMax = 0;
    for (int j = 0; j < imgHeight; j++) {
        for (int i = 0; i < imgWidth; i++) {
            if (countMax < countOfPoint[j * imgWidth + i]) {
                countMax = countOfPoint[j * imgWidth + i];
            }
        }
    }
    countMax = qMax<int>(countMax,10);
    dataImg->fill(Qt::transparent);
    QPainter painterDataImg(dataImg);
    painterDataImg.setPen(Qt::NoPen);
    painterDataImg.setPen(Qt::transparent);
    for (int j = 0; j < imgHeight; j++) {
        for (int i = 0; i < imgWidth ; i++) {
            int index = j * imgWidth + i;
            //qDebug() << countOfPoint[index];

            const uchar alpha = uchar(qBound<qreal>(0,countOfPoint[index] * 255 * 1.2 / qreal(countMax), 255));
            if (alpha == 0) {
                continue;
            }
            QRadialGradient gradient(i, j, pointSizeV2);
            gradient.setColorAt(0, QColor(0, 0, 0, alpha));
            gradient.setColorAt(1, QColor(0, 0, 0, 0));
            painterDataImg.setBrush(gradient);
            painterDataImg.drawEllipse(QPointF(i, j), pointSizeV2, pointSizeV2);
        }
    }

    for (int row = 0; row < dataImg->height(); row++) {
        const uchar *line_data = dataImg->scanLine(row);
        QRgb *line_heat = reinterpret_cast<QRgb*>(heatImg->scanLine(row));
        for (int col = 0; col < dataImg->width(); col++)
        {
            line_heat[col] = colorList[line_data[col]];
        }
    }

    sceneV2->addObject(new ImageObject(
                           Position(left + lonPerGrid * gridLowerX, bottom + latPerGrid * gridLowerY),
                           Position(left + lonPerGrid * (gridUpperX + 1), bottom + latPerGrid * (gridUpperY + 1)),
                           *heatImg
                           )
                       );
/*
    QDialog* debugWindow = new QDialog(this);
    QVBoxLayout* l = new QVBoxLayout(debugWindow);
    debugWindow->setLayout(l);
    QGraphicsScene* sceneDebug = new QGraphicsScene(debugWindow);
    QGraphicsView * viewDebug = new QGraphicsView(sceneDebug);
    sceneDebug->addItem(new QGraphicsPixmapItem(QPixmap::fromImage(*dataImg)));

    l->addWidget(viewDebug);
    debugWindow->show();
*/


    delete [] countOfPoint;
}


void VisualizationDialog::on_heatMapV2ButtonRight_clicked()
{
    int step = heatMapSliderV2->value();
    if (step == (endTime - startTime) / timeStepV2) {
        return;
    }
    heatMapSliderV2->setValue(step + 1);
    UpdateHeatMapV2();
}

void VisualizationDialog::on_heatMapV2ButtonLeft_clicked()
{
    int step = heatMapSliderV2->value();
    if (step == 0) {
        return;
    }
    heatMapSliderV2->setValue(step - 1);
    UpdateHeatMapV2();
}
