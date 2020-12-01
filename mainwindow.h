#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "DataEntry.h"
#include "rangeslider.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void ReceiveShow(std::vector<DataEntry>* dataFrame, std::vector<std::vector<double>>* grid);
private:
    Ui::MainWindow *ui;
    std::vector<DataEntry>* dataFrame = nullptr;
    std::vector<std::vector<double>>* grid = nullptr;
    RangeSlider* timeSpanSlider;
    RangeSlider* fieldsLngSlider;
    RangeSlider* fieldsLatSlider;
    long long lowerTimeBound;
    long long upperTimeBound;
    long long startTime;
    long long endTime;


private slots:
    void SetStartTimeFromEdit(const QDateTime &tmpDateTime);
    void SetEndTimeFromEdit(const QDateTime &tmpDateTime);
};

#endif // MAINWINDOW_H

