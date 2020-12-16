#include "predictmodel.h"
#include <cmath>

PredictModel::PredictModel(QString modelPath)
{
    // load
    QFile loadFile(modelPath);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qDebug() << "fail to open model file";
        return;
    }

    QByteArray allData = loadFile.readAll();
    loadFile.close();

    QJsonParseError json_error;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(allData));

    if (json_error.error != QJsonParseError::NoError) {
        qDebug() << "json error";
        return;
    }

    QJsonObject rootObj = jsonDoc.object();

    QStringList keys = rootObj.keys();
    for (int i = 0; i < keys.size(); i++) {
        qDebug() << "key" << i << "is:" << keys.at(i);
    }
    // init model
    for (int i = 0; i < 8; i++) {
        // weight
        QJsonArray weightArray = rootObj.value("features." + QString::number(i) + ".weight").toArray();
        int size = weightArray.size();
        for (int j = 0; j < size; j++) {
            weight[i].push_back(QVector<qreal>());
            QJsonArray vec = weightArray[j].toArray();
            for (int k = 0; k < vec.size(); k++) {
                QJsonValue value = vec[k];
                if (value.type() == QJsonValue::Double) {
                    weight[i][j].push_back(value.toDouble());
                } else {
                    qDebug() << "error double";
                }
            }

            for (int k = 0; k < vec.size(); k++) {
                //qDebug() << weight[i][j][k];
            }
        }
        qDebug() << "weight size1" << weight[i].size();
        qDebug() << "weight size2" << weight[i][0].size();
        // bias
        QJsonArray biasArray = rootObj.value("features." + QString::number(i) + ".bias").toArray();
        for (int j = 0; j < biasArray.size(); j++) {
            bias[i].push_back(QVector<qreal>());
            QJsonValue value = biasArray[j];
            if (value.type() == QJsonValue::Double) {
                bias[i][j].push_back(value.toDouble());
            } else {
                qDebug() << "error double";
            }
        }
        qDebug() << "bias size1" << bias[i].size();
        qDebug() << "bias size2" << bias[i][0].size();
    }
}

int PredictModel::Predict(QPointF orig, QPointF dest, long long departureTime, int weekday)
{
    // get input vector
    Matrix input = ConvertToVec(orig, dest, departureTime, weekday);
    Matrix x = input;
    // forward
    for (int i = 0; i < 7; i++) {
        x = Plus(Multiply(weight[i], x), bias[i]);
        x = ReLU(x);
    }
    x = Plus(Multiply(weight[7], x), bias[7]);
    qreal sum = 0;
    for (int i = 0; i < x.size(); i++) {
        sum += x[i][0];
    }
    qDebug() << "predict" << sum;

    // get the prediction
    int result = int(sum * 761.08 + 1280.060) / 60;
    if (result < 0 || result > 300) {
        return  -1;
    } else {
        return result;
    }
}

PredictModel::Matrix PredictModel::ConvertToVec(QPointF orig, QPointF dest, long long departureTime, int weekday)
{
    PredictModel::Matrix tmp;
    for (int i = 0; i < 25; i++) {
        tmp.push_back(QVector<qreal>());
        tmp[i].push_back(0);
    }
    //qDebug() << orig.x() << ' ' << orig.y() << ' ' << dest.x() << ' ' << dest.y();
    //qDebug() << departureTime << ' ' << weekday;
    // time
    tmp[0][0] = (departureTime - startTimeMean) / startTimeMean;
    int dayIndex = weekday;
    if (dayIndex == 0 || dayIndex == 6) {
        tmp[2][0] = 1;
    } else {
        tmp[1][0] = 1;
    }

    // orig
    tmp[3][0] = ((orig.x() - leftLon) - origLonMean) / origLonStd;
    tmp[4][0] = ((orig.y() - bottomLat) - origLatMean) / origLatStd;
    int groupIndex = 0;
    qreal r = sqrt((orig.x() - midLon) * (orig.x() - midLon) + (orig.y() - midLat) * (orig.y() - midLat));
    for (int i = 0; i < 8; i++) {
        if (r > radius[i]) {
            groupIndex++;
        } else {
            break;
        }
    }
    tmp[5 + groupIndex][0] = 1;
    tmp[14][0] = ((dest.x() - leftLon) - destLonMean) / destLonStd;
    tmp[15][0] = ((dest.y() - bottomLat) - destLatMean) / destLatStd;
    groupIndex = 0;
    r = sqrt((dest.x() - midLon) * (dest.x() - midLon) + (dest.y() - midLat) * (dest.y() - midLat));
    for (int i = 0; i < 8; i++) {
        if (r > radius[i]) {
            groupIndex++;
        } else {
            break;
        }
    }
    tmp[16 + groupIndex][0] = 1;
    QString a = "";
    for (int i = 0; i < 25; i++) {
        a += QString::number(tmp[i][0]) + ' ';
    }
    qDebug() << a;
    return tmp;
}

PredictModel::Matrix PredictModel::Multiply(const Matrix& a, const Matrix& b)
{
    int aSize1 = a.size();
    int aSize2 = a[0].size();
    int bSize1 = b.size();
    int bSize2 = b[0].size();
    if (aSize2 != bSize1) {
        qDebug() << "matrix multiply error";
        return Matrix();
    }
               //qDebug() << aSize1 << ' ' << aSize2 << ' ' << bSize1 << ' ' << bSize2;
    Matrix tmp;
    for (int i = 0; i < aSize1; i++) {
        tmp.push_back(QVector<qreal>());
        for (int j = 0; j < bSize2; j++) {

            qreal sum = 0;
            for (int k = 0; k < aSize2; k++) {
                sum += a[i][k] * b[k][j];
            }
            tmp[i].push_back(sum);
        }
    }
    return tmp;
}

PredictModel::Matrix PredictModel::Plus(const Matrix& a, const Matrix& b)
{
    int aSize1 = a.size();
    int aSize2 = a[0].size();
    int bSize1 = b.size();
    int bSize2 = a[0].size();
    if (aSize1 != bSize1 || aSize2 != bSize2) {
        qDebug() << "matrix plus error";
    }
    Matrix tmp;
    for (int i = 0; i < aSize1; i++) {
        tmp.push_back(QVector<qreal>());
        for (int j = 0; j < aSize2; j++) {
            tmp[i].push_back(a[i][j] + b[i][j]);
        }
    }
    return tmp;
}

PredictModel::Matrix PredictModel::ReLU(const Matrix& a)
{
    int size1 = a.size();
    int size2 = a[0].size();
    Matrix tmp;
    for (int i = 0; i < size1; i++) {
        tmp.push_back(QVector<qreal>());
        for (int j = 0; j < size2; j++) {
            tmp[i].push_back(qMax<qreal>(a[i][j], 0));
        }
    }
    return tmp;
}

int PredictModel::Predict(Matrix& input)
{
    Matrix x = input;
    // forward
    for (int i = 0; i < 7; i++) {
        x = Plus(Multiply(weight[i], x), bias[i]);
        x = ReLU(x);
    }

    x = Plus(Multiply(weight[7], x), bias[7]);
    qreal sum = 0;
    for (int i = 0; i < x.size(); i++) {
        sum += x[i][0];
    }
    qDebug() << "predict" << sum;

    // get the prediction
    int result = int(sum * 761.08 + 1280.060) / 60;
    if (result < 0 || result > 300) {
        return  -1;
    } else {
        return result;
    }
}
