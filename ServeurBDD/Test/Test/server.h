#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>

class MyServer : public QObject {
    Q_OBJECT
public:
    explicit MyServer(quint16 port, QObject *parent = nullptr);
    ~MyServer();

private slots:
    void onNewConnection();
    void processTextMessage(QString message);
    void socketDisconnected();

private:
    QWebSocketServer *m_pWebSocketServer;
    QList<QWebSocket *> m_clients;
};

#endif
