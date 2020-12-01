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
