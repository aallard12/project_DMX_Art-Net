#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantList>
#include <QBuffer>
#include <QDataStream>
#include <QByteArray>

class NetworkManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionChanged)

public:
    explicit NetworkManager(QObject *parent = nullptr);
    bool isConnected();

    Q_INVOKABLE void fetchUniverses();
    Q_INVOKABLE void fetchScenes();
    Q_INVOKABLE void fetchSceneValues(int idScene);
    Q_INVOKABLE void connectToTcpServer(const QString &ip, int port);
    Q_INVOKABLE void lancerSceneTCP(int idScene);
    Q_INVOKABLE void setDmxValueTCP(int univers, int projecteur, int canal, int valeur);

signals:
    void connectionChanged();
    void universesReceived(QVariantList data);
    void scenesReceived(QVariantList data);
    void sceneValuesReceived(QVariantList data);

    void messageLogged(QString direction, QString content);

private slots:
    void onTcpReadyRead();

private:
    QTcpSocket tcpSocket;
    QByteArray tcpBuffer;
};

#endif
