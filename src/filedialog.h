#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include "DataEntry.h"
#include "fileloadthread.h"
#include "rangeslider.h"
#include <QDialog>
#include <vector>
#include <QDir>
#include <QFileDialog>
#include <QStringList>
#include <QMessageBox>

namespace Ui {
class FileDialog;
}

class FileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileDialog(QWidget *parent = nullptr);
    ~FileDialog();

private:
    Ui::FileDialog *ui;
    QString path;
    QVector<DataEntry>* dataFrame = nullptr;
    QVector<QVector<qreal>>* grid = nullptr;
    FileLoadThread thread;

    int startTime = 1;
    int endTime = 15;
    RangeSlider* timeSlider;
public:

private slots:
    // set the init folder for user to select the dataset
    void SetFolderPath();
    // start reading data from csv file in another thread
    void ReadFileList();
    // deal with wrong file directory
    void WrongInput();
    // Show the dialog leading to the mainwindow
    void LoadingFinished();
    // reset progressbar for another try
    void Reset();
    //
    void UpdateTime(int startTime, int endTime);
signals:
    void SendData(QVector<DataEntry>* dataFrame, QVector<QVector<qreal>>*);
};

#endif // FILEDIALOG_H
