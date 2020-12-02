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
    QVector<DataEntry>* dataFrame = nullptr;
    QVector<QVector<qreal>>* grid = nullptr;
    FileLoadThread thread;

public:

private slots:
    void SetFolderPath();
    void ReadFileList();
    void WrongInput();
    void LoadingFinished();
    void Reset();

signals:
    void SendData(QVector<DataEntry>* dataFrame, QVector<QVector<qreal>>*);
};

#endif // FILEDIALOG_H
