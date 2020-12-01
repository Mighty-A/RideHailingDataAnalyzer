#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MapGraphicsScene.h"
#include "MapGraphicsObject.h"
#include "MapGraphicsView.h"
#include "tileSources/OSMTileSource.h"
#include "tileSources/CompositeTileSource.h"

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
    connect(timeSpanSlider, &RangeSlider::sliderReleased, this, &MainWindow::SetTimeFromSlider);

    // set Map
    mapBox->resize(500, 500);
    QVBoxLayout *mapLayout = new QVBoxLayout();
    mapBox->setLayout(mapLayout);

    MapGraphicsScene * scene = new MapGraphicsScene(this);
    MapGraphicsView * view = new MapGraphicsView(scene,this);
    mapLayout->addWidget(view);

    QSharedPointer<OSMTileSource> osmTiles(new OSMTileSource(OSMTileSource::OSMTiles), &QObject::deleteLater);
    QSharedPointer<CompositeTileSource> composite(new CompositeTileSource(), &QObject::deleteLater);
    composite->addSourceBottom(osmTiles);

    view->setTileSource(composite);

    // map init
    view->setZoomLevel(4);
    view->centerOn(104.07, 30.67);

}
MainWindow::~MainWindow()
{
    delete ui;
    delete dataFrame;
    delete grid;
}


void MainWindow::ReceiveShow(std::vector<DataEntry>* dataFrame, std::vector<std::vector<double>>* grid)
{
    this->dataFrame = dataFrame;
    this->grid = grid;
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
