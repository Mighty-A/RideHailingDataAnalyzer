#ifndef FILELOADTHREAD_H
#define FILELOADTHREAD_H

#include "DataEntry.h"


#include <QThread>
#include <QMutex>
#include <QTextStream>
#include <QFile>

class FileLoadThread : public QThread
{
    Q_OBJECT
public:
    FileLoadThread();
    void LoadFile(QString path, std::vector<DataEntry>* dataFrame, std::vector<std::vector<double>>* grid);

private:
    QString path;
    std::vector<DataEntry>* dataFrame;
    std::vector<std::vector<double>>* grid;

protected:
    void run() override;
    QMutex mutex;

signals:
    void UpdateProgressBar(int newValue);
    void FailToRead();
    void LoadingFinished();
};

#endif // FILELOADTHREAD_H
