/**
 * @file networkmanager.cpp
 * @brief Implémentation du NetworkManager avec logs détaillés et complets.
 */

#include "networkmanager.h"
#include <QDebug>
#include <QJsonArray>

NetworkManager::NetworkManager(QObject *parent) : QObject(parent) {
    connect(&tcpSocket, &QTcpSocket::connected, [this](){
        emit messageLogged("SYS", "Connecté");
        emit connectionChanged();
    });
    connect(&tcpSocket, &QTcpSocket::disconnected, [this](){
        emit messageLogged("SYS", "Déconnecté");
        emit connectionChanged();
    });
    connect(&tcpSocket, &QTcpSocket::readyRead, this, &NetworkManager::onTcpReadyRead);
}

bool NetworkManager::isConnected() {
    return tcpSocket.state() == QAbstractSocket::ConnectedState;
}

void NetworkManager::connectToTcpServer(const QString &ip, int port) {
    emit messageLogged("SYS", QString("Connexion %1:%2").arg(ip).arg(port));
    if (tcpSocket.state() == QAbstractSocket::ConnectedState) tcpSocket.disconnectFromHost();
    tcpSocket.connectToHost(ip, port);
}

void NetworkManager::fetchUniverses() {
    if (isConnected()) {
        QJsonObject json;
        json["action"] = "get_universes";
        emit messageLogged("SEND", "GET Univers");
        tcpSocket.write(QJsonDocument(json).toJson(QJsonDocument::Compact) + "\n");
    }
}

void NetworkManager::fetchScenes() {
    if (isConnected()) {
        QJsonObject json;
        json["action"] = "get_scenes";
        emit messageLogged("SEND", "GET Scènes");
        tcpSocket.write(QJsonDocument(json).toJson(QJsonDocument::Compact) + "\n");
    }
}

void NetworkManager::fetchSceneValues(int idScene) {
    if (isConnected()) {
        QJsonObject json;
        json["action"] = "get_scene_values";
        json["idScene"] = idScene;
        emit messageLogged("SEND", QString("GET Valeur Scène:%1").arg(idScene));
        tcpSocket.write(QJsonDocument(json).toJson(QJsonDocument::Compact) + "\n");
    }
}

void NetworkManager::onTcpReadyRead() {
    tcpBuffer.append(tcpSocket.readAll());
    while (tcpBuffer.contains('\n')) {
        int index = tcpBuffer.indexOf('\n');
        QByteArray message = tcpBuffer.left(index).trimmed();
        tcpBuffer.remove(0, index + 1);

        // --- MODIFICATION : On envoie le message entier ---
        emit messageLogged("RECV", QString::fromUtf8(message));

        QJsonDocument doc = QJsonDocument::fromJson(message);
        if (doc.isArray()) {
            QJsonArray arr = doc.array();
            if (!arr.isEmpty()) {
                QJsonObject first = arr[0].toObject();
                if (first.contains("nom_univers")) emit universesReceived(arr.toVariantList());
                else if (first.contains("nomScene")) emit scenesReceived(arr.toVariantList());
                else if (first.contains("valeurCanaux")) emit sceneValuesReceived(arr.toVariantList());
            }
        }
    }
}

void NetworkManager::lancerSceneTCP(int idScene) {
    if (isConnected()) {
        QJsonObject json;
        json["commande"] = "P";
        json["idScene"] = idScene;
        emit messageLogged("SEND", QString("PLAY Scène:%1").arg(idScene));
        tcpSocket.write(QJsonDocument(json).toJson(QJsonDocument::Compact) + "\n");
    }
}

void NetworkManager::setDmxValueTCP(int univers, int projecteur, int canal, int valeur) {
    if (isConnected()) {
        char commande = 'C';
        QBuffer tampon;
        tampon.open(QIODevice::WriteOnly);
        QDataStream out(&tampon);
        out << (quint16)0 << commande << (quint8)univers << (quint16)projecteur << (quint16)canal << (quint8)valeur;
        quint16 taille = static_cast<quint16>(tampon.size()) - 2;
        tampon.seek(0);
        out << taille;

        // --- MODIFICATION : Libellés abrégés ---
        emit messageLogged("SEND", QString("DMX Univers:%1 Projecteur:%2 Canal:%3 Valeur:%4").arg(univers).arg(projecteur).arg(canal).arg(valeur));
        tcpSocket.write(tampon.buffer());
    }
}
