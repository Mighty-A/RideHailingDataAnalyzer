#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include "DataEntry.h"
#include "fileloadthread.h"
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
    std::vector<DataEntry>* dataFrame = nullptr;
    std::vector<std::vector<double>>* grid = nullptr;
    FileLoadThread thread;

private slots:
    void SetFolderPath();
    void ReadFileList();
    void WrongInput();
    void LoadingFinished();

signals:
    void SendData(std::vector<DataEntry>* dataFrame, std::vector<std::vector<double>>*);
};

#endif // FILEDIALOG_H
