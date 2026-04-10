#include "server.h"
#include <QDebug>

MyServer::MyServer(quint16 port, QObject *parent) : QObject(parent),
    m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Serveur RPi"),
    QWebSocketServer::NonSecureMode, this)) {

    if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
        qDebug() << "Attente du PC sur le port" << port;
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &MyServer::onNewConnection);
    }
}

MyServer::~MyServer() {
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

void MyServer::onNewConnection() {
    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();
    qDebug() << "PC connecte :" << pSocket->peerAddress().toString();

    // Correction du signal : textMessageReceived est bien un signal de QWebSocket
    connect(pSocket, &QWebSocket::textMessageReceived, this, &MyServer::processTextMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &MyServer::socketDisconnected);

    m_clients << pSocket;
}

void MyServer::processTextMessage(QString message) {
    qDebug() << "Message recu :" << message;
    // Si tu envoies "P,1", cela s'affichera ici [cite: 607]
}

void MyServer::socketDisconnected() {
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient) {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
        qDebug() << "PC deconnecte";
    }
}
