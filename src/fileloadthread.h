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
    bool _SUCCESS = false;
    FileLoadThread();
    void LoadFile(QString path, QVector<DataEntry>* dataFrame, QVector<QVector<qreal>>* grid);
    void Cancel();

private:
    QString path;
    QVector<DataEntry>* dataFrame;
    QVector<QVector<qreal>>* grid;

protected:
    void run() override;
    QMutex mutex;

signals:
    void UpdateProgressBar(int newValue);
    void FailToRead();
    void LoadingFinished();
};

#endif // FILELOADTHREAD_H
