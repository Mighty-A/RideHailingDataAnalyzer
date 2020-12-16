#include "predictiondialog.h"
#include "ui_predictiondialog.h"

PredictionDialog::PredictionDialog(QWidget *qparent, MainWindow* parent)
 :
    mainwindow(parent),
    QDialog(qparent),
    ui(new Ui::PredictionDialog)
{
    ui->setupUi(this);
    this->dataFrame = mainwindow->dataFrame;
    this->grid = mainwindow->grid;
    this->dataInGrid = mainwindow->dataInGrid;
    this->manager = mainwindow->manager;

    scene = new MapGraphicsScene(this);
    view = new MapGraphicsView(scene, this);
    ui->mapLayout->addWidget(view);

    QSharedPointer<OSMTileSource> osmTiles(new OSMTileSource(OSMTileSource::OSMTiles), &QObject::deleteLater);
    QSharedPointer<MyTileSource> rectTiles(new MyTileSource(), &QObject::deleteLater);
    QSharedPointer<CompositeTileSource> composite(new CompositeTileSource(), &QObject::deleteLater);
    composite->addSourceBottom(osmTiles);
    composite->addSourceTop(rectTiles);
    view->setTileSource(composite);
    connect(ui->weekdayComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &PredictionDialog::SetWeekday);
    view->setZoomLevel(11);
    view->centerOn(104.0652259999995, 30.65897999999995);

    connect(ui->origEdit, &QLineEdit::textEdited, this, &PredictionDialog::SendOrigTipsRequest);
    connect(ui->destEdit, &QLineEdit::textEdited, this, &PredictionDialog::SendDestTipsRequest);
    connect(manager, &NetworkManager::AddressTipFinished, this, &PredictionDialog::ReceiveTips);
    connect(ui->origEdit, &QLineEdit::returnPressed, ui->OrigButton, &QPushButton::click);
    connect(ui->destEdit, &QLineEdit::returnPressed, ui->DestButton, &QPushButton::click);
    connect(ui->timeEdit, &QTimeEdit::timeChanged, this, &PredictionDialog::SetTime);
    completer = new QCompleter(this);
    stringListModelForCompleter = new QStringListModel(this);
    completer->setModel(stringListModelForCompleter);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setMaxVisibleItems(10);
    ui->origEdit->setCompleter(completer);
    ui->destEdit->setCompleter(completer);
    ui->predictButton->setEnabled(true);

    origPoint = new PointObject(orig, 0.00313, 1.0, QColor(255, 0, 0, 100));
    destPoint = new PointObject(dest, 0.00313, 1.0, QColor(0, 255, 0, 100));
    //scene->addObject(origPoint);
    //scene->addObject(destPoint);
    QImage icon1 = QPixmap(":/icon/icon1.png").toImage().mirrored(false, true);
    QImage icon2 = QPixmap(":/icon/icon2.png").toImage().mirrored(false, true);

    origIcon = new ImageObject(Position(orig.x() - icon_size / 2, orig.y()), Position(orig.x() + icon_size / 2, orig.y() + icon_size), icon1);
    destIcon = new ImageObject(Position(dest.x() - icon_size / 2, dest.y()), Position(dest.x() + icon_size / 2, dest.y() + icon_size), icon2);
    scene->addObject(origIcon);
    scene->addObject(destIcon);
    /*
    QDialog* debugWindow = new QDialog(this);
    QVBoxLayout* l = new QVBoxLayout(debugWindow);
    debugWindow->setLayout(l);
    QGraphicsScene* sceneDebug = new QGraphicsScene(debugWindow);
    QGraphicsView * viewDebug = new QGraphicsView(sceneDebug);
    sceneDebug->addItem(new QGraphicsPixmapItem(QPixmap::fromImage(icon1)));

    l->addWidget(viewDebug);
    debugWindow->show();
   */
}

PredictionDialog::~PredictionDialog()
{
    delete ui;
}

void PredictionDialog::SetWeekday(int w)
{
    currentWeekday = w;
}

void PredictionDialog::SendOrigTipsRequest(QString key)
{
    //qDebug() << '1';
    manager->GetAddressTip(QUrl("http://restapi.amap.com/v3/assistant/inputtips?output=json&location=104.0652,30.6590&city=028"
                     "&keywords="+key+"&key=244a48ad9141aa8b29a7860bdbdc65c9"));
}

void PredictionDialog::SendDestTipsRequest(QString key)
{
    manager->GetAddressTip(QUrl("http://restapi.amap.com/v3/assistant/inputtips?output=json&location=104.0652,30.6590&city=028"
                     "&keywords="+key+"&key=244a48ad9141aa8b29a7860bdbdc65c9"));
}

void PredictionDialog::ReceiveTips(QStringList tip, QVector<QPointF> location)
{
    tips = tip;
    stringListModelForCompleter->setStringList(tips);
    locations = location;
    ui->origEdit->setText(ui->origEdit->text());
    ui->destEdit->setText(ui->destEdit->text());
}


void PredictionDialog::on_OrigButton_clicked()
{
    if (!tips.contains(ui->origEdit->text())) {
        return;
    }
    else {

        int id = tips.indexOf(ui->origEdit->text());
        //qDebug() << id;
        orig = locations[id];

    }
    UpdateMap();
}

void PredictionDialog::on_DestButton_clicked()
{
    if (!tips.contains(ui->destEdit->text())) {
        return;
    }
    else {
        int id = tips.indexOf(ui->destEdit->text());
        dest = locations[id];
    }
    UpdateMap();
}


void PredictionDialog::UpdateMap() {
    origPoint->setPoint(orig);
    destPoint->setPoint(dest);
    origIcon->SetRect(Position(orig.x() - icon_size / 2, orig.y()), Position(orig.x() + icon_size / 2, orig.y() + icon_size));
    destIcon->SetRect(Position(dest.x() - icon_size / 2, dest.y()), Position(dest.x() + icon_size / 2, dest.y() + icon_size));
}

void PredictionDialog::SetTime(QTime time)
{
    currentTime = QTime(0, 0).secsTo(time);
}

qreal PredictionDialog::dist(const QPointF& a, const QPointF& b) {
    return qSqrt((a.x() - b.x()) * (a.x() - b.x()) + (a.y() - b.y()) * (a.y() - b.y()));
}
void PredictionDialog::on_predictButton_clicked()
{
    // todo

    // statistic
    int origGridIndex = LocatePointInGrid(orig);
    QVector<const DataEntry*> tmpV;
    for (int i = 0; i < (*dataInGrid)[origGridIndex][1].size(); i++) {
        const DataEntry* tmp = (*dataInGrid)[origGridIndex][1][i];
        if (dist(tmp->orig, orig) < latlonOffset && dist(tmp->dest, dest) < latlonOffset
                && qAbs<long long>(tmp->departureTime % (24 * 3600) - currentTime) < timeOffset) {
            tmpV.push_back(tmp);
        }
    }
    //qDebug() << '1';
    for (int i = 0; i < (*dataInGrid)[origGridIndex][2].size(); i++) {
        const DataEntry* tmp = (*dataInGrid)[origGridIndex][2][i];
        if (dist(tmp->orig, orig) < latlonOffset && dist(tmp->dest, dest) < latlonOffset
            && qAbs<long long >(tmp->departureTime % (24 * 3600) - currentTime) < timeOffset) {
            tmpV.push_back(tmp);
        }
    }
    //qDebug() << '1';
    int sum = 0;
    for (int i = 0; i < tmpV.size(); i++) {
        //qDebug() << tmpV[i]->endTime - tmpV[i]->departureTime;
        sum += tmpV[i]->endTime - tmpV[i]->departureTime;
    }
    if (tmpV.size() == 0) {
        ui->queryLabel->setText("No similar order!");
    } else {
        ui->queryLabel->setText("Found " + QString::number(tmpV.size(), 10) + " entries\nAverage time:"
                                + QString::number(sum / tmpV.size() / 60, 10) + "minutes");
    }



    // machinelearning
    ui->PredictLabel->setText("DNN predict: " + QString::number(20) + "minutes");
}


