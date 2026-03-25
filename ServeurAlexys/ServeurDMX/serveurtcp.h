#ifndef SERVEURTCP_H
#define SERVEURTCP_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QDataStream>

class ServeurTCP : public QObject
{
    Q_OBJECT

public:

    explicit ServeurTCP(QObject *parent = nullptr);

public slots:
    void onQTcpServer_newConnection();
    void onQTcpServer_readyRead();
    void onQTcpServer_disconnected();

private:
    QTcpServer *socketServer;
};

#endif // SERVEURTCP_H
