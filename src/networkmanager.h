#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QtNetwork>
#include <QPixmap>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

class NetworkManager: public QObject
{
    Q_OBJECT
public:
    NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();

    void GetAddressTip(QUrl u);
signals:
    void AddressTipFinished(QStringList, QVector<QPointF>);

public slots:
    void AddressTipsReceived();
private:
    QNetworkAccessManager manager;
    QUrl url;
    QNetworkReply *reply;
};

#endif // NETWORKMANAGER_H
