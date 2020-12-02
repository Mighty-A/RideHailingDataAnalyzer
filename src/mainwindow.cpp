#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#include <QtDebug>

long long FromDateTimetoInt(QDateTime t) {
    const long long OFFSET = - 8 * 3600;
    return t.toMSecsSinceEpoch() / 1000 + OFFSET;
}
QDateTime FromInttoDateTime(long long time) {
    const long long OFFSET = 8 * 3600;
    QDateTime tmp;
    tmp.setMSecsSinceEpoch((time + OFFSET) * 1000);
    return tmp;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // set RangeSlider
    timeSpanSlider = new RangeSlider(Qt::Orientation::Vertical);
    fieldsLngSlider = new RangeSlider(Qt::Orientation::Horizontal);
    fieldsLatSlider = new RangeSlider(Qt::Orientation::Vertical);
    timeSpanSlider->SetRange(0, 359);
    fieldsLngSlider->SetRange(0, 9);
    fieldsLatSlider->SetRange(0, 9);
    ui->gridLayout->addItem(new QSpacerItem(10, 10),1 ,0);
    ui->gridLayout->addWidget(fieldsLngSlider, 1, 1);
    ui->gridLayout->addWidget(fieldsLatSlider, 0, 0);
    ui->startAndEndLayout->addWidget(timeSpanSlider);
    ui->gridLayout->addWidget(mapBox = new QGroupBox("Map"), 0, 1);
    ui->gridLayout->setColumnStretch(0, 0);
    ui->gridLayout->setColumnStretch(1, 1);
    ui->gridLayout->setRowStretch(0, 1);
    ui->gridLayout->setRowStretch(1, 0);

    // set Time
    lowerTimeBound = startTime = FromDateTimetoInt(ui->startDateTimeEdit->dateTime());
    upperTimeBound = endTime = FromDateTimetoInt(ui->endDateTimeEdit->dateTime());

    connect(ui->startDateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &MainWindow::SetStartTimeFromEdit);
    connect(ui->endDateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &MainWindow::SetEndTimeFromEdit);
    connect(timeSpanSlider, &RangeSlider::valueChanged, this, &MainWindow::SetTimeFromSlider);

    // set Map
    mapBox->resize(500, 500);
    QVBoxLayout *mapLayout = new QVBoxLayout();
    mapBox->setLayout(mapLayout);

    MapGraphicsScene * scene = new MapGraphicsScene(this);
    MapGraphicsView * view = new MapGraphicsView(scene,this);
    mapLayout->addWidget(view);

    QSharedPointer<OSMTileSource> osmTiles(new OSMTileSource(OSMTileSource::OSMTiles), &QObject::deleteLater);
    QSharedPointer<MyTileSource> rectTiles(new MyTileSource(), &QObject::deleteLater);
    //QSharedPointer<GridTileSource> gridTiles(new GridTileSource(), &QObject::deleteLater);
    QSharedPointer<CompositeTileSource> composite(new CompositeTileSource(), &QObject::deleteLater);
    composite->addSourceBottom(osmTiles);
    composite->addSourceTop(rectTiles);
    view->setTileSource(composite);

    //view->setDragMode(MapGraphicsView::DragMode::NoDrag);


    // map init
    view->setZoomLevel(10);
    view->centerOn(104.0652259999995, 30.65897999999995);
    for (int i = 0; i < 4; i++) {
        Rect[i] = new LineObject(Position(0, 0), Position(0, 0));
        scene->addObject(Rect[i]);
    }

    connect(this, &MainWindow::SetRect, rectTiles.data(), &MyTileSource::SetRect);
    connect(fieldsLngSlider, &RangeSlider::valueChanged, this, &MainWindow::SetFieldsFromHorizontalSliders);
    connect(fieldsLatSlider, &RangeSlider::valueChanged, this, &MainWindow::SetFieldsFromVerticalSliders);
    // refresh
    connect(rectTiles.data(), &MyTileSource::rectChanged, view, &MapGraphicsView::Update);
}
MainWindow::~MainWindow()
{
    delete ui;
    delete dataFrame;
    delete grid;
}


void MainWindow::ReceiveShow(QVector<DataEntry>* dataFrame, QVector<QVector<qreal>>* grid)
{
    this->dataFrame = dataFrame;
    this->grid = grid;
    //qDebug() << dataFrame << dataFrame->size();
    //qDebug() << grid << grid->size() << ' ' << grid[0].size();

    SetRect(QPointF((*grid)[0][6], (*grid)[0][7]), QPointF((*grid)[99][2], (*grid)[99][3]));

    emit SetRect(QPointF((*grid)[0][6], (*grid)[0][7]), QPointF((*grid)[99][2], (*grid)[99][3]));
    //view->setZoomLevel(view->zoomLevel());
    this->show();

}

void MainWindow::SetStartTimeFromEdit(const QDateTime& tmpDateTime) {
    long long tmp = FromDateTimetoInt(tmpDateTime);
    if (tmp == startTime) {
        return;
    }
    if (tmp > endTime) {
        QMessageBox tmpMess;
        tmpMess.setText(tr("Wrong Time Span Input! Please Input a Valid Time Span"));\
        tmpMess.exec();
        // ui->startDateTimeEdit->setDateTime(FromInttoDateTime(lowerTimeBound)); no reset
        return;
    }
    startTime = tmp;
    timeSpanSlider->setLowerValue(int((tmp - lowerTimeBound) / 3600));
}

void MainWindow::SetEndTimeFromEdit(const QDateTime& tmpDateTime) {
    long long tmp = FromDateTimetoInt(tmpDateTime);
    if (tmp == endTime) {
        return;
    }
    if (tmp < startTime) {
        QMessageBox tmpMess;
        tmpMess.setText(tr("Wrong Time Span Input! Please Input a Valid Time Span"));
        //ui->endDateTimeEdit->setDateTime(FromInttoDateTime(upperTimeBound));  no reset
        tmpMess.exec();
        return;
    }
    endTime = tmp;
    timeSpanSlider->setUpperValue(int((tmp - lowerTimeBound)/ 3600));
}

void MainWindow::SetTimeFromSlider(int aLowerValue, int aUpperValue)
{
    // set time hourly
    startTime = lowerTimeBound + aLowerValue * 3600;
    endTime = lowerTimeBound + aUpperValue * 3600;
    ui->startDateTimeEdit->setDateTime(FromInttoDateTime(startTime));
    ui->endDateTimeEdit->setDateTime(FromInttoDateTime(endTime));
    return;
}


void MainWindow::SetFieldsFromHorizontalSliders(int lower, int upper) {


    gridLowerX = lower;
    gridUpperX = upper;
    SetRect(
            QPointF((*grid)[gridLowerX + gridLowerY * 10][6], (*grid)[gridLowerX + gridLowerY * 10][7]),
            QPointF((*grid)[gridUpperX + gridUpperY * 10][2], (*grid)[gridUpperX + gridUpperY * 10][3])
        );/*
    view->centerOn(
            ((*grid)[gridLowerX + gridLowerY * 10][6] + (*grid)[gridUpperX + gridUpperY * 10][2]) / 2,
            ((*grid)[gridLowerX + gridLowerY * 10][7] + (*grid)[gridUpperX + gridUpperY * 10][3]) / 2
                );*/
}
void MainWindow::SetFieldsFromVerticalSliders(int lower, int upper) {
    qDebug() << "Vetical change. from " << gridLowerY << ' ' << gridUpperY << " to " << 9 - upper << ' ' << 9 - lower;
    gridLowerY = 9 - upper;
    gridUpperY = 9 - lower;
    SetRect(
            QPointF((*grid)[gridLowerX + gridLowerY * 10][6], (*grid)[gridLowerX + gridLowerY * 10][7]),
            QPointF((*grid)[gridUpperX + gridUpperY * 10][2], (*grid)[gridUpperX + gridUpperY * 10][3])
        );/*
    view->centerOn(
            ((*grid)[gridLowerX + gridLowerY * 10][6] + (*grid)[gridUpperX + gridUpperY * 10][2]) / 2,
            ((*grid)[gridLowerX + gridLowerY * 10][7] + (*grid)[gridUpperX + gridUpperY * 10][3]) / 2
                );*/
}

void MainWindow::SetRect(QPointF bottomLeft, QPointF topRight) {
    qDebug() << "new rect: (" << gridLowerX << "," << gridLowerY << ") (" << gridUpperX << "," << gridUpperY << ")";
    qDebug() << "(" << bottomLeft.x() << "," << bottomLeft.y() << ") (" << topRight.x() << "," << topRight.y() << ")";
    Rect[0]->setEndPointA(Position(bottomLeft.x(), bottomLeft.y()));
    Rect[0]->setEndPointB(Position(topRight.x(), bottomLeft.y()));
    Rect[1]->setEndPointA(Position(topRight.x(), bottomLeft.y()));
    Rect[1]->setEndPointB(Position(topRight.x(), topRight.y()));
    Rect[2]->setEndPointA(Position(topRight.x(), topRight.y()));
    Rect[2]->setEndPointB(Position(bottomLeft.x(), topRight.y()));
    Rect[3]->setEndPointA(Position(bottomLeft.x(), topRight.y()));
    Rect[3]->setEndPointB(Position(bottomLeft.x(), bottomLeft.y()));
}
