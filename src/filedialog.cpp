#include "filedialog.h"
#include "ui_filedialog.h"

#include <QDebug>

FileDialog::FileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileDialog)
{
    ui->setupUi(this);

    //init FileDialog, set path
    this->setWindowTitle("Load File");
    path = QDir::currentPath();
    ui->folderpath->setText(path);
    dataFrame = new QVector<DataEntry>;
    grid = new QVector<QVector<qreal>>;
    ui->CancelButton->setEnabled(false);

    // slider
    timeSlider = new RangeSlider(Qt::Orientation::Horizontal);
    timeSlider->SetRange(1, 15);
    ui->groupBox->layout()->addWidget(timeSlider);
    connect(timeSlider, &RangeSlider::valueChanged, this, &FileDialog::UpdateTime);

    connect(ui->openDirButton, &QPushButton::released, this, &FileDialog::SetFolderPath);

    // read
    connect(ui->OKButton, &QPushButton::released, this, &FileDialog::ReadFileList);
    connect(&thread, &FileLoadThread::FailToRead, this, &FileDialog::WrongInput);
    connect(ui->CancelButton, &QPushButton::released, &thread, &FileLoadThread::Cancel);
    connect(&thread, &FileLoadThread::UpdateProgressBar, ui->loadProgressBar, &QProgressBar::setValue);
    connect(&thread, &FileLoadThread::LoadingFinished, this, &FileDialog::LoadingFinished);
    connect(&thread, &FileLoadThread::finished, this, &FileDialog::Reset);
}

FileDialog::~FileDialog()
{
    delete ui;
}

void FileDialog::SetFolderPath()
{
    path = QFileDialog::getExistingDirectory(this, "Choose Data File Directory", "/");
    ui->folderpath->setText(path);
}

void FileDialog::ReadFileList()
{
    ui->OKButton->setEnabled(false);
    ui->CancelButton->setEnabled(true);
    thread.LoadFile(path, dataFrame, grid, startTime, endTime);
}

void FileDialog::WrongInput()
{
    Reset();
    QMessageBox tmp;
    tmp.setText("Wrong File Folder! Please choose the correct folder of data!");
    tmp.exec();
    return;
}

void FileDialog::LoadingFinished()
{
    QMessageBox::StandardButton result;
    //qDebug() << dataFrame << dataFrame->size();
    //qDebug() << grid << grid->size() << ' ' << grid[0].size();
    result = QMessageBox::question(this, "Successfully Loaded", "Go to the main window?", QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes) {
        emit SendData(dataFrame, grid);
        hide();
    } else {
        thread._SUCCESS = false;
        Reset();
    }
}

void FileDialog::Reset()
{
    if (thread._SUCCESS)
        return;
    ui->loadProgressBar->reset();
    grid->clear();
    dataFrame->clear();
    ui->OKButton->setEnabled(true);
    ui->CancelButton->setEnabled(false);
}

void FileDialog::UpdateTime(int startTime, int endTime)
{
    this->startTime = startTime;
    this->endTime = endTime;
    ui->timeLabel->setText("Time: 2016.11." + QString::number(startTime) + " - 2016.11." + QString::number(endTime));
}
