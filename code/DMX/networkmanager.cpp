/**
 * @file networkmanager.cpp
 * @brief Implémentation de la classe NetworkManager.
 * @author Valentin
 * @date 2026
 * @details Gère l'implémentation de la logique réseau TCP (connexion,
 * formatage JSON, construction de trames binaires et buffer de réception).
 */

#include "networkmanager.h"
#include <QJsonArray>
#include <QJsonObject>

NetworkManager::NetworkManager(QObject *parent) : QObject(parent) {
    // Connexion des signaux natifs de QTcpSocket vers nos signaux personnalisés
    connect(&tcpSocket, &QTcpSocket::connected, [this](){
        emit connectionChanged();
    });
    connect(&tcpSocket, &QTcpSocket::disconnected, [this](){
        emit connectionChanged();
    });

    // Branchement de la réception asynchrone des paquets TCP
    connect(&tcpSocket, &QTcpSocket::readyRead, this, &NetworkManager::onTcpReadyRead);
}

bool NetworkManager::isConnected() {
    return tcpSocket.state() == QAbstractSocket::ConnectedState;
}

void NetworkManager::connectToTcpServer(const QString &ip, int port) {
    // Sécurité : On s'assure de couper une éventuelle ancienne connexion avant d'en rouvrir une
    if (tcpSocket.state() == QAbstractSocket::ConnectedState) {
        tcpSocket.disconnectFromHost();
    }
    tcpSocket.connectToHost(ip, port);
}

void NetworkManager::fetchUniverses() {
    if (isConnected()) {
        QJsonObject json;
        json["action"] = "get_universes";
        // L'ajout du saut de ligne '\n' est crucial : il sert de délimiteur de fin de trame pour le serveur
        tcpSocket.write(QJsonDocument(json).toJson(QJsonDocument::Compact) + "\n");
    }
}

void NetworkManager::fetchScenes(int idUnivers) {
    if (isConnected()) {
        QJsonObject json;
        json["action"] = "get_scenes";
        json["idUnivers"] = idUnivers;
        tcpSocket.write(QJsonDocument(json).toJson(QJsonDocument::Compact) + "\n");
    }
}

void NetworkManager::fetchSceneValues(int idScene) {
    if (isConnected()) {
        QJsonObject json;
        json["action"] = "get_scene_values";
        json["idScene"] = idScene;
        tcpSocket.write(QJsonDocument(json).toJson(QJsonDocument::Compact) + "\n");
    }
}

void NetworkManager::sendLoginTCP(const QString &username, const QString &password) {
    if (isConnected()) {
        QJsonObject json;
        json["action"] = "login";
        json["username"] = username;
        json["password"] = password;
        tcpSocket.write(QJsonDocument(json).toJson(QJsonDocument::Compact) + "\n");
    }
}

void NetworkManager::onTcpReadyRead() {
    // 1. Solution anti-fragmentation : On ajoute les données brutes au tampon global.
    // Le réseau TCP peut couper un long JSON en plusieurs petits morceaux.
    tcpBuffer.append(tcpSocket.readAll());

    // 2. Traitement des paquets complets : Tant qu'il y a un '\n', on a un message entier.
    while (tcpBuffer.contains('\n')) {
        int index = tcpBuffer.indexOf('\n');

        // Extraction du message complet (sans le \n)
        QByteArray message = tcpBuffer.left(index).trimmed();

        // Nettoyage du tampon pour le prochain paquet
        tcpBuffer.remove(0, index + 1);

        // 3. Tentative de conversion en JSON
        QJsonDocument doc = QJsonDocument::fromJson(message);

        // Cas A : Le message est un objet simple (ex: réponse de login)
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            if (obj.contains("action") && obj["action"].toString() == "login_response") {
                bool success = (obj["status"].toString() == "success");
                emit loginResponseReceived(success);
            }
        }
        // Cas B : Le message est un tableau (ex: listes de données de la base)
        else if (doc.isArray()) {
            QJsonArray arr = doc.array();
            if (!arr.isEmpty()) {
                // On vérifie le contenu du premier élément pour deviner le type de liste
                QJsonObject first = arr[0].toObject();

                if (first.contains("nom_univers")) {
                    emit universesReceived(arr.toVariantList());
                }
                else if (first.contains("nomScene")) {
                    emit scenesReceived(arr.toVariantList());
                }
                else if (first.contains("valeurCanaux")) {
                    emit sceneValuesReceived(arr.toVariantList());
                }
            }
        }
    }
}

void NetworkManager::lancerSceneTCP(int idScene) {
    if (isConnected()) {
        QJsonObject json;
        json["commande"] = "P";
        json["idScene"] = idScene;
        tcpSocket.write(QJsonDocument(json).toJson(QJsonDocument::Compact) + "\n");
    }
}

void NetworkManager::setDmxValueTCP(int univers, int projecteur, int canal, int valeur) {
    if (isConnected()) {
        char commande = 'C'; // Commande 'Curseur'

        // Utilisation d'un QBuffer en mémoire pour préparer la trame binaire proprement
        QBuffer tampon;
        tampon.open(QIODevice::WriteOnly);
        QDataStream out(&tampon);

        // Écriture séquentielle avec typage strict (quint8, quint16)
        // L'entête de taille (0 pour l'instant) réserve la place des 2 premiers octets
        out << (quint16)0 << commande << (quint8)univers << (quint16)projecteur << (quint16)canal << (quint8)valeur;

        // Calcul de la taille réelle des données utiles (taille totale - les 2 octets d'en-tête)
        quint16 taille = static_cast<quint16>(tampon.size()) - 2;

        // Retour au début du buffer pour y inscrire la vraie taille
        tampon.seek(0);
        out << taille;

        // Envoi direct de la trame binaire sur la socket TCP (Fire and Forget)
        tcpSocket.write(tampon.buffer());
    }
}
