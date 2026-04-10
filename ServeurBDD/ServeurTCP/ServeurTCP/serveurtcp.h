#ifndef SERVEURTCP_H
#define SERVEURTCP_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QSqlDatabase>
#include <QList>

class ServeurTcp : public QObject {
    Q_OBJECT
public:
    explicit ServeurTcp(quint16 port, QObject *parent = nullptr);
    ~ServeurTcp();

private slots:
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnected();

private:
    bool setupDatabase();
    void sendArtNetPacket(int univers, const QString& ip, const QByteArray& dmxData);

    QTcpServer *m_tcpServer;
    QList<QTcpSocket *> m_clients;
    QUdpSocket *m_udpSocket; // Client UDP pour envoyer l'Art-Net
    QSqlDatabase m_db;
};

#endif // SERVEURTCP_H
