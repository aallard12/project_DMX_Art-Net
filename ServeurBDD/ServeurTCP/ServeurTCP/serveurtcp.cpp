#include "serveurtcp.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

ServeurTcp::ServeurTcp(quint16 port, QObject *parent) : QObject(parent),
    m_tcpServer(new QTcpServer(this)),
    m_udpSocket(new QUdpSocket(this)) {

    if (!setupDatabase()) {
        qCritical() << "ERREUR : Impossible de connecter la BDD.";
    }

    if (m_tcpServer->listen(QHostAddress::Any, port)) {
        qDebug() << "Serveur TCP en ecoute sur le port" << port;
        connect(m_tcpServer, &QTcpServer::newConnection, this, &ServeurTcp::onNewConnection);
    } else {
        qCritical() << "ERREUR : Le serveur TCP n'a pas pu demarrer.";
    }
}

ServeurTcp::~ServeurTcp() {
    m_tcpServer->close();
    if (m_db.isOpen()) m_db.close();
}

bool ServeurTcp::setupDatabase() {
    m_db = QSqlDatabase::addDatabase("QMYSQL");
    m_db.setHostName("127.0.0.1");
    m_db.setDatabaseName("DMXBDD");
    m_db.setUserName("root");
    m_db.setPassword("raspberry");
    return m_db.open();
}

void ServeurTcp::onNewConnection() {
    QTcpSocket *clientSocket = m_tcpServer->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::readyRead, this, &ServeurTcp::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &ServeurTcp::onClientDisconnected);
    m_clients << clientSocket;
    qDebug() << "Nouveau client TCP connecte :" << clientSocket->peerAddress().toString();
}

void ServeurTcp::onReadyRead() {
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    QString message = QString::fromUtf8(data).trimmed();
    qDebug() << "Commande recue :" << message;

    // Analyse de la trame "P,idScene"
    QStringList parts = message.split(',');
    if (parts.size() >= 2 && parts[0] == "P") {
        int idScene = parts[1].toInt();

        if (!m_db.isOpen()) {
            qDebug() << "La BDD est fermee, tentative de reconnexion...";
            m_db.open();
        }

        QSqlQuery query;
        query.prepare("SELECT U.numeroUnivers, U.adresseIp, E.adresseDepart, C.numeroCanal, P.valeurCanaux "
                      "FROM PILOTE P "
                      "JOIN CANAUX C ON P.idCanal = C.idCanal "
                      "JOIN EQUIPEMENTS E ON C.idEquipement = E.idEquipement "
                      "JOIN UNIVERS U ON E.idUnivers = U.idUnivers "
                      "WHERE P.idScene = :idScene");
        query.bindValue(":idScene", idScene);

        if (query.exec()) {
            // Buffers pour 3 univers (un par IP d'ESP32)
            QMap<QString, QByteArray> dmxBuffers;
            QMap<QString, int> ipToUniv;
            QStringList ips = {"192.168.1.31", "192.168.1.32", "192.168.1.33"};

            for(const QString& ip : ips) dmxBuffers[ip] = QByteArray(512, 0);

            while (query.next()) {
                QString ip = query.value("adresseIp").toString();
                int offset = query.value("adresseDepart").toInt();
                int canal = query.value("numeroCanal").toInt();
                int valeur = query.value("valeurCanaux").toInt();
                ipToUniv[ip] = query.value("numeroUnivers").toInt();

                int index = offset + canal - 2; // Conversion index 0-511
                if (index >= 0 && index < 512 && dmxBuffers.contains(ip)) {
                    dmxBuffers[ip][index] = static_cast<char>(valeur);
                }
            }

            // Envoi Art-Net vers chaque ESP32
            for (auto it = dmxBuffers.begin(); it != dmxBuffers.end(); ++it) {
                sendArtNetPacket(ipToUniv[it.key()], it.key(), it.value());
            }
            qDebug() << "Art-Net envoye pour la scene" << idScene;
        } else {
            qDebug() << "Erreur SQL :" << query.lastError().text();
        }
    }
}

void ServeurTcp::sendArtNetPacket(int univers, const QString& ip, const QByteArray& dmxData) {
    QByteArray packet;
    // 1. ID : "Art-Net" suivi d'un 0 (8 octets)
    packet.append("Art-Net", 8);

    // 2. OpCode : ArtDmx (0x5000) -> Little Endian donc 00 puis 50
    packet.append((char)0x00);
    packet.append((char)0x50);

    // 3. Protocol Version (v14) -> Big Endian donc 00 puis 0E
    packet.append((char)0x00);
    packet.append((char)0x0E);

    // 4. Sequence (0x00 pour désactiver)
    packet.append((char)0x00);

    // 5. Physical (Port physique 0x00)
    packet.append((char)0x00);

    // 6. Universe (Port-Address) -> LITTLE ENDIAN
    // Octet 14 (Low) : Univers (0-15)
    // Octet 15 (High) : Subnet (0-15) et Net (0-127)
    packet.append(static_cast<char>(univers & 0xFF));        // Low Byte
    packet.append(static_cast<char>((univers >> 8) & 0x7F)); // High Byte (7 bits pour le Net)

    // 7. Length : Taille des données DMX (512) -> BIG ENDIAN (contrairement au reste !)
    // 512 = 0x0200 -> Octet 16 : 02, Octet 17 : 00
    packet.append((char)0x02);
    packet.append((char)0x00);

    // 8. Données DMX (512 octets)
    packet.append(dmxData);

    m_udpSocket->writeDatagram(packet, QHostAddress(ip), 6454);
}
void ServeurTcp::onClientDisconnected() {
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket) {
        m_clients.removeAll(socket);
        socket->deleteLater();
        qDebug() << "Client TCP deconnecte.";
    }
}
