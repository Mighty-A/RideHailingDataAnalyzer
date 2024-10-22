#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#include <QtDebug>

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
    QSharedPointer<CompositeTileSource> composite(new CompositeTileSource(), &QObject::deleteLater);
    composite->addSourceBottom(osmTiles);
    composite->addSourceTop(rectTiles);
    view->setTileSource(composite);

    // map parameters
    view->setZoomLevel(11);
    view->centerOn(104.0652259999995, 30.65897999999995);
    for (int i = 0; i < 4; i++) {
        Rect[i] = new LineObject(Position(0, 0), Position(0, 0));
        scene->addObject(Rect[i]);
    }

    connect(fieldsLngSlider, &RangeSlider::valueChanged, this, &MainWindow::SetFieldsFromHorizontalSliders);
    connect(fieldsLatSlider, &RangeSlider::valueChanged, this, &MainWindow::SetFieldsFromVerticalSliders);
    connect(rectTiles.data(), &MyTileSource::rectChanged, view, &MapGraphicsView::Update);

    // data holder init
    dataInGrid = new QVector<QVector<QVector<const DataEntry*>>>;
    for (int i = 0; i < 100; i++) {
        QVector<QVector<const DataEntry*>> tmp;
        for (int j = 0; j < 3; j++) {
            tmp.push_back(QVector<const DataEntry*>());
        }
        dataInGrid->push_back(tmp);
    }
    pthread = new QThread();
    preprocess = new Worker();
    preprocess->moveToThread(pthread);
    connect(this, &MainWindow::Preprocess, preprocess, &Worker::slt_dowork);
    connect(preprocess, &Worker::sig_finish, this, &MainWindow::PreprocessFinished);
    pthread->start();

    // analyzer button init
    ui->graphButton->setEnabled(false);
    ui->visualButton->setEnabled(false);
    ui->predictButton->setEnabled(false);
    setWindowIcon(QIcon(":/icon/icon/taxi.svg"));

    // network
    manager = new NetworkManager(this);
}
MainWindow::~MainWindow()
{
    delete ui;
    delete dataFrame;
    delete grid;
    pthread->quit();
    pthread->wait();
}


void MainWindow::ReceiveShow(QVector<DataEntry>* dataFrame, QVector<QVector<qreal>>* grid)
{
    this->show();
    mutex.lock();
    this->grid = grid;
    this->dataFrame = dataFrame;
    ui->statusbar->showMessage(tr("Preprocessing..."));
    emit Preprocess(dataFrame, grid, dataInGrid);

    mutex.unlock();
    SetRect(QPointF((*grid)[0][6], (*grid)[0][7]), QPointF((*grid)[99][2], (*grid)[99][3]));
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
        );
}
void MainWindow::SetFieldsFromVerticalSliders(int lower, int upper) {
    gridLowerY = 9 - upper;
    gridUpperY = 9 - lower;
    SetRect(
            QPointF((*grid)[gridLowerX + gridLowerY * 10][6], (*grid)[gridLowerX + gridLowerY * 10][7]),
            QPointF((*grid)[gridUpperX + gridUpperY * 10][2], (*grid)[gridUpperX + gridUpperY * 10][3])
        );
}

void MainWindow::SetRect(QPointF bottomLeft, QPointF topRight) {
    Rect[0]->setEndPointA(Position(bottomLeft.x(), bottomLeft.y()));
    Rect[0]->setEndPointB(Position(topRight.x(), bottomLeft.y()));
    Rect[1]->setEndPointA(Position(topRight.x(), bottomLeft.y()));
    Rect[1]->setEndPointB(Position(topRight.x(), topRight.y()));
    Rect[2]->setEndPointA(Position(topRight.x(), topRight.y()));
    Rect[2]->setEndPointB(Position(bottomLeft.x(), topRight.y()));
    Rect[3]->setEndPointA(Position(bottomLeft.x(), topRight.y()));
    Rect[3]->setEndPointB(Position(bottomLeft.x(), bottomLeft.y()));
}

void MainWindow::PreprocessFinished() {
    ui->statusbar->showMessage(tr("Ready"));
    ui->graphButton->setEnabled(true);
    ui->visualButton->setEnabled(true);
    ui->predictButton->setEnabled(true);
}

Worker::Worker(QObject *parent) {
    Q_UNUSED(parent)
}

Worker::~Worker()
{

}

void Worker::slt_dowork(
        QVector<DataEntry>* dataFrame,
        QVector<QVector<qreal>>* grid,
        QVector<QVector<QVector<const DataEntry*>>>* dataInGrid
)
{
    for (int i = 0; i < dataFrame->size(); i++) {
        int indexOrig = LocatePointInGrid((*dataFrame)[i].orig, grid);
        int indexDest = LocatePointInGrid((*dataFrame)[i].dest, grid);
        const DataEntry* tmp = &(dataFrame->at(i));
        if (indexDest == indexOrig) {
            (*dataInGrid)[indexOrig][dataInGridType::INTERNAL].push_back(tmp);
        } else {

            (*dataInGrid)[indexOrig][dataInGridType::OUTFLOW].push_back(tmp);
            (*dataInGrid)[indexDest][dataInGridType::INFLOW].push_back(tmp);
        }
    }
    emit sig_finish();
}

void MainWindow::on_graphButton_clicked()
{
    GraphDialog* graphWindow = new GraphDialog(this, this);
    graphWindow->show();
}

void MainWindow::on_visualButton_clicked()
{
    VisualizationDialog* visualizationDialog = new VisualizationDialog(this, this);
    visualizationDialog->show();
}

void MainWindow::on_predictButton_clicked()
{
    PredictionDialog* predictWindow = new PredictionDialog(this, this);
    predictWindow->show();
}
