#include "mainwindow.h"
#include "ui_mainwindow.h"

long long FromDateTimetoInt(QDateTime t) {
    const long long OFFSET = - 8 * 3600;
    return t.toMSecsSinceEpoch() / 1000 + OFFSET;
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
    timeSpanSlider->setEnabled(false);
    fieldsLngSlider->SetRange(0, 9);
    fieldsLatSlider->SetRange(0, 9);
    ui->gridLayout->addItem(new QSpacerItem(5, 5),0 ,0);
    ui->gridLayout->addWidget(fieldsLngSlider, 0, 1, 1, 10);
    ui->gridLayout->addWidget(fieldsLatSlider, 1, 0, 10, 1);
    ui->startAndEndLayout->addWidget(timeSpanSlider);
    ui->gridLayout->addItem(new QSpacerItem(100, 100), 1, 1);

    // set Time
    lowerTimeBound = startTime = FromDateTimetoInt(ui->startDateTimeEdit->dateTime());
    upperTimeBound = endTime = FromDateTimetoInt(ui->endDateTimeEdit->dateTime());
    connect(ui->startDateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &MainWindow::SetStartTimeFromEdit);
    connect(ui->endDateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &MainWindow::SetEndTimeFromEdit);


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
    startTime = tmp;
    timeSpanSlider->setLowerValue(int((tmp - lowerTimeBound) / 3600));
}

void MainWindow::SetEndTimeFromEdit(const QDateTime& tmpDateTime) {
    long long tmp = FromDateTimetoInt(tmpDateTime);
    if (tmp == endTime) {
        return;
    }
    endTime = tmp;
    timeSpanSlider->setUpperValue(int((tmp - lowerTimeBound)/ 3600));
}
