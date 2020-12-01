#include "fileloadthread.h"

FileLoadThread::FileLoadThread()
{

}

void FileLoadThread::LoadFile(QString path, std::vector<DataEntry> *dataFrame, std::vector<std::vector<double> > *grid)
{
    mutex.lock();
    this->path = path;
    this->dataFrame = dataFrame;
    this->grid = grid;
    mutex.unlock();
    _CAN_RUN = true;
    start();
    //wait();
}

void FileLoadThread::run()
{
    const int NUMBER_OF_FILES = 75;
    mutex.lock();
    QString path = this->path;
    std::vector<DataEntry>* dataFrame = this->dataFrame;
    std::vector<std::vector<double>>* grid = this->grid;
    mutex.unlock();
    // load grid data

    QFile gridFile(path + "/rectangle_grid_table.csv");
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
    emit UpdateProgressBar(5);

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
                        split[1].toLongLong(),
                        split[2].toLongLong(),
                        Point{split[3].toDouble(), split[4].toDouble()},
                        Point{split[5].toDouble(), split[6].toDouble()},
                        split[7].toDouble()
                );
                dataFrame->push_back(tmp);
            }
            count += 1;
            emit UpdateProgressBar(int(5 + double(count) / NUMBER_OF_FILES * 95));

            // check for stop signal
            {
                QMutexLocker lock(&mutex);
                if (!_CAN_RUN) {
                    return;
                }
            }
        }
    }
    emit LoadingFinished();
}


void FileLoadThread::Cancel() {
    QMutexLocker locker(&mutex);
    _CAN_RUN = false;
}