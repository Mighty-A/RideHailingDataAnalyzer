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
    bool _CAN_RUN = true;

    FileLoadThread();
    void LoadFile(QString path, std::vector<DataEntry>* dataFrame, std::vector<std::vector<double>>* grid);
    void Cancel();

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
