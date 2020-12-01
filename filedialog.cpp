#include "filedialog.h"
#include "ui_filedialog.h"

FileDialog::FileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileDialog)
{
    ui->setupUi(this);

    //init FileDialog, set path
    this->setWindowTitle("Load File");
    path = QDir::currentPath();
    ui->folderpath->setText(path);
    dataFrame = new std::vector<DataEntry>;
    grid = new std::vector<std::vector<double>>;
    ui->CancelButton->setEnabled(false);

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
    thread.LoadFile(path, dataFrame, grid);
    /*
    const int NUMBER_OF_FILES = 75;

    // load grid data
    QFile gridFile(path + "/rectangle_grid_table.csv", this);
    if (!gridFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit FailToRead();
        return;
    }
    QTextStream stream_text(&gridFile);
    QStringList gridLines;
    while (!stream_text.atEnd()) {
        gridLines.push_back(stream_text.readLine());
    }
    QStringList col = gridLines[0].split(',');
    for (int j = 1; j < gridLines.size(); j++) {
        QString line = gridLines[j];
        QStringList split = line.split(',');
        std::vector<double> tmp;

        for (int col = 1; col < split.size(); col++) {
            tmp.push_back(split[col].toDouble());
        }
        grid->push_back(tmp);
    }
    ui->loadProgressBar->setValue(5);

    // load order data
    const QStringList Day = {"01", "02", "03", "04", "05", "06", "07", "08",
                             "09", "10", "11", "12", "13", "14", "15"};
    int count = 0;
    for (int i = 0; i < Day.length(); i++) {
        for (int j = 0; j < 5; j ++) {
            QString debug = path + "/order_201611" + Day[i] + "_part" + QString(j + '0') + ".csv";
            QFile dataFile(path + "/order_201611" + Day[i] + "_part" + QString(j + '0') + ".csv");
            if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                emit FailToRead();
                return;
            }
            QTextStream streamText(&dataFile);
            QStringList dataLines;
            while (!streamText.atEnd()) {
                dataLines.push_back(streamText.readLine());
            }
            for (int k = 1; k < dataLines.size(); k++) {
                QStringList split = dataLines[k].split(',');
                DataEntry tmp(
                        split[0],
                        split[1].toInt(),
                        split[2].toInt(),
                        Point{split[3].toDouble(), split[4].toDouble()},
                        Point{split[5].toDouble(), split[6].toDouble()},
                        split[7].toDouble()
                );
                dataFrame->push_back(tmp);
            }
            count += 1;
            ui->loadProgressBar->setValue(int(5 + double(count) / NUMBER_OF_FILES * 95));
        }
    }
    emit LoadingFinished(dataFrame, grid);*/
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
    result = QMessageBox::question(this, "Successfully Loaded", "Go to the main window?", QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes) {
        emit SendData(dataFrame, grid);
        close();
    } else {
        Reset();
    }
}

void FileDialog::Reset()
{

    ui->loadProgressBar->reset();
    grid->clear();
    dataFrame->clear();
    ui->OKButton->setEnabled(true);
    ui->CancelButton->setEnabled(false);
}
