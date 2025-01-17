#include "networkmanager.h"

NetworkManager::NetworkManager(QObject* parent):
    QObject(parent)
{
    reply =  nullptr;
    qRegisterMetaType<QVector<QPair<double,double>>>("QVector<QPointF>");
    QNetworkRequest request;
    request.setUrl(QUrl("http://restapi.amap.com/v3/assistant/inputtips?output=json&location=104.0652,30.6590&city=028"
                        "&keywords=成都&key=244a48ad9141aa8b29a7860bdbdc65c9"));
    reply = manager.get(request);
}

NetworkManager::~NetworkManager()
{

}

void NetworkManager::GetAddressTip(QUrl u)
{
    QNetworkRequest request;
    request.setUrl(u);
    if (reply != nullptr) {
        reply->deleteLater();
    }
    reply = manager.get(request);
    connect(reply, &QNetworkReply::finished, this, &NetworkManager::AddressTipsReceived);
}

void NetworkManager::AddressTipsReceived()
{
    QStringList tips;
    QByteArray data = reply->readAll();
    QJsonParseError json_error;
    QJsonDocument doc(QJsonDocument::fromJson(data, &json_error));

    if (json_error.error != QJsonParseError::NoError) {
        qDebug() << "Json Parse Error";
    }
    QVector <QPointF> locations;
    QJsonObject Obj = doc.object();
    QJsonArray arr = Obj.value("tips").toArray();
    for (int i = 0; i < arr.size(); i++) {
        QJsonObject tmp = Obj.value("tips")[i].toObject();
        if (tmp.value("location").toString().isEmpty())
            continue;
        QStringList tmpstr = tmp.value("location").toString().split(",");
        tips.push_back(tmp.value("name").toString());
        locations.append(QPointF(tmpstr[0].toDouble(), tmpstr[1].toDouble()));
    }
    emit AddressTipFinished(tips, locations);
}
