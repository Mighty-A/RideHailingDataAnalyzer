#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QJsonDocument>
#include <QPointF>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>

class PredictModel
{
public:
    typedef QVector<QVector<qreal>> Matrix;
private:
    const qreal bottomLat = 30.524081949676;
    const qreal topLat = 30.7938780503239;
    const qreal leftLon = 103.908407474531;
    const qreal rightLon = 104.222044525468;
    const long long startTime = 1477929600;
    const long long endTime = startTime + 1296000;
    const qreal startTimeMean = 52240.2989764033;
    const qreal startTimeStd = 19467.1600675313;
    const qreal origLonMean = 0.161370855045317;
    const qreal origLonStd = 0.0391185328560802;
    const qreal origLatMean = 0.150093485852388;
    const qreal origLatStd = 0.0353629847333074;
    const qreal destLonMean = 0.161292627300344;
    const qreal destLonStd = 0.0398233707683038;
    const qreal destLatMean = 0.149508261654321;
    const qreal destLatStd = 0.0359904735316399;
    const qreal midLat = (bottomLat + topLat) / 2;
    const qreal midLon = (leftLon + rightLon) / 2;
    const qreal radius[8] = {0.019667478, 0.029940668, 0.040076348, 0.0611749, 0.080543666, 0.091776206, 0.11, 0.133019267};



    Matrix weight[8];
    Matrix bias[8];

    Matrix ConvertToVec(QPointF orig, QPointF dest, long long startTime, int weekday);
    Matrix Multiply(const Matrix& a, const Matrix& b);
    Matrix Plus(const Matrix& a, const Matrix& b);
    Matrix ReLU(const Matrix& a);

public:
    PredictModel(QString modelPath);


    int Predict(QPointF orig, QPointF dest, long long startTime, int weekday);
    int Predict(Matrix& x);
};

#endif // MODEL_H
