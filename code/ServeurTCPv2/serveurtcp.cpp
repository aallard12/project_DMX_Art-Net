/**
 * @file serveurtcp.cpp
 * @brief Implémentation de ServeurTcp avec API JSON intégrée.
 * @version 2.2 (Final)
 * @author Alexys LAFFIAC
 * @date 2026
 */

#include "serveurtcp.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDataStream>
#include <QCryptographicHash> // ✨ INDISPENSABLE POUR LE HACHAGE DU MOT DE PASSE

/// @brief Initialise le serveur TCP, le socket UDP et la connexion MariaDB.
ServeurTcp::ServeurTcp(quint16 port, QObject *parent) : QObject(parent),
    m_tcpServer(new QTcpServer(this)),
    m_udpSocket(new QUdpSocket(this)) {

    if (!setupDatabase()) {
        qCritical() << "ERREUR BDD : Verifiez vos identifiants MariaDB.";
    }

    if (m_tcpServer->listen(QHostAddress::Any, port)) {
        qDebug() << "Serveur TCP pret sur le port" << port;
        connect(m_tcpServer, &QTcpServer::newConnection, this, &ServeurTcp::onNewConnection);
    } else {
        qCritical() << "ERREUR : Impossible d'ouvrir le port" << port;
    }
}

/// @brief Ferme le serveur TCP et la base de données.
ServeurTcp::~ServeurTcp() {
    m_tcpServer->close();
    if (m_db.isOpen()) m_db.close();
}

/// @brief Configure et ouvre la connexion QMYSQL vers DMXBDD.
bool ServeurTcp::setupDatabase() {
    m_db = QSqlDatabase::addDatabase("QMYSQL");
    m_db.setHostName("127.0.0.1");
    m_db.setDatabaseName("DMXBDD");
    m_db.setUserName("root");
    m_db.setPassword("raspberry");
    return m_db.open();
}

/// @brief Accepte le client entrant et connecte ses signaux.
void ServeurTcp::onNewConnection() {
    QTcpSocket *clientSocket = m_tcpServer->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::readyRead, this, &ServeurTcp::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &ServeurTcp::onClientDisconnected);
    m_clients << clientSocket;
    qDebug() << "Tablette connectee :" << clientSocket->peerAddress().toString();
}

/// @brief Dispatche la trame reçue (JSON API ou Binaire DMX).
void ServeurTcp::onReadyRead() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();

    // 1. Décodage du JSON
    if (data.trimmed().startsWith('{')) {
        QList<QByteArray> messages = data.split('\n');
        for (const QByteArray& msg : messages) {
            if (msg.trimmed().isEmpty()) continue;

            QJsonDocument doc = QJsonDocument::fromJson(msg);
            if (!doc.isNull() && doc.isObject()) {
                traiterRequeteJson(socket, doc.object());
            }
        }
        return;
    }

    // 2. Traitement du flux binaire (Curseurs DMX)
    QDataStream in(data);
    while (!in.atEnd()) {
        if (in.device()->bytesAvailable() < 3) break;

        quint16 taille;
        qint8 cmd_raw;
        in >> taille >> cmd_raw;
        char commande = static_cast<char>(cmd_raw);

        if (commande == 'C') {
            if (in.device()->bytesAvailable() < 6) break;
            quint8 uni; quint16 proj; quint16 can; quint8 val;
            in >> uni >> proj >> can >> val;
            envoyerCurseur(uni, proj, can, val);
        } else {
            break;
        }
    }
}

// ========================================================
// ROUTEUR JSON
// ========================================================

/// @brief Analyse le JSON et déclenche la bonne action.
void ServeurTcp::traiterRequeteJson(QTcpSocket *client, const QJsonObject &json) {
    if (json.contains("action")) {
        QString action = json["action"].toString();

        if (action == "get_universes") {
            envoyerReponseUniverses(client);
        }
        else if (action == "get_scenes") {
            envoyerReponseScenes(client);
        }
        else if (action == "get_scene_values") {
            int idScene = json["idScene"].toInt();
            envoyerReponseSceneValues(client, idScene);
        }
        else if (action == "login") {
            QString user = json["username"].toString();
            QString mdp = json["password"].toString();
            verifierLogin(client, user, mdp);
        }
    }
    else if (json.contains("commande") && json["commande"].toString() == "P") {
        int idScene = json["idScene"].toInt();
        qDebug() << "JSON Recu -> Lancement ID Scene:" << idScene;
        envoyerScene(idScene);
    }
}

// ========================================================
// REQUÊTES SQL -> JSON VERS TABLETTE
// ========================================================

/// @brief Vérifie l'authentification en base de données avec hachage sécurisé.
void ServeurTcp::verifierLogin(QTcpSocket *client, const QString &user, const QString &mdp) {
    // ✨ CORRECTION INTÉGRÉE : Hachage SHA-256 du mot de passe clair pour matcher avec la BDD
    QString mdpChiffre = QString(QCryptographicHash::hash(mdp.toUtf8(), QCryptographicHash::Sha256).toHex());

    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM USERS WHERE nomUtilisateur = :user AND mdp = :mdp");
    query.bindValue(":user", user);
    query.bindValue(":mdp", mdpChiffre); // On utilise la version chiffrée pour la requête

    QJsonObject response;
    response["action"] = "login_response";

    if (query.exec() && query.next()) {
        if (query.value(0).toInt() > 0) {
            response["status"] = "success";
            qDebug() << "✅ Login REUSSI pour l'utilisateur :" << user;
        } else {
            response["status"] = "failed";
            qDebug() << "❌ Login ECHOUE pour l'utilisateur :" << user;
        }
    } else {
        response["status"] = "error";
        qDebug() << "❌ ERREUR SQL Login :" << query.lastError().text();
    }

    client->write(QJsonDocument(response).toJson(QJsonDocument::Compact) + "\n");
}

/// @brief Renvoie la liste des scènes.
void ServeurTcp::envoyerReponseScenes(QTcpSocket *client) {
    QJsonArray jsonArray;
    QSqlQuery query("SELECT idScene, nomScene FROM SCENES");

    while (query.next()) {
        QJsonObject scene;
        scene["idScene"] = query.value("idScene").toInt();
        scene["nomScene"] = query.value("nomScene").toString();
        jsonArray.append(scene);
    }

    client->write(QJsonDocument(jsonArray).toJson(QJsonDocument::Compact) + "\n");
}

/// @brief Renvoie les valeurs des canaux d'une scène.
void ServeurTcp::envoyerReponseSceneValues(QTcpSocket *client, int idScene) {
    QJsonArray jsonArray;
    QSqlQuery query;
    query.prepare("SELECT U.idUnivers, E.idEquipement, C.numeroCanal, P.valeurCanaux "
                  "FROM PILOTE P "
                  "JOIN CANAUX C ON P.idCanal = C.idCanal "
                  "JOIN EQUIPEMENTS E ON C.idEquipement = E.idEquipement "
                  "JOIN UNIVERS U ON E.idUnivers = U.idUnivers "
                  "WHERE P.idScene = :id");
    query.bindValue(":id", idScene);
    query.exec();

    while (query.next()) {
        QJsonObject val;
        val["idUnivers"] = query.value("idUnivers").toInt();
        val["idEquipement"] = query.value("idEquipement").toInt();
        val["numeroCanal"] = query.value("numeroCanal").toInt();
        val["valeurCanaux"] = query.value("valeurCanaux").toInt();
        jsonArray.append(val);
    }

    client->write(QJsonDocument(jsonArray).toJson(QJsonDocument::Compact) + "\n");
}

/// @brief Renvoie l'arborescence complète (Univers -> Equipements -> Canaux -> Fonctionnalités).
void ServeurTcp::envoyerReponseUniverses(QTcpSocket *client) {
    QJsonArray jsonUniverses;
    QSqlQuery queryUniv("SELECT * FROM UNIVERS");

    if (!queryUniv.isActive()) {
        qDebug() << "❌ ERREUR SQL (Univers) :" << queryUniv.lastError().text();
    }

    while (queryUniv.next()) {
        QJsonObject univers;
        int idUniv = queryUniv.value("idUnivers").toInt();
        if (idUniv == 0) idUniv = queryUniv.value("id").toInt();

        QString ipUnivers = queryUniv.value("adresseIp").toString().trimmed();
        int numUnivers = queryUniv.value("numeroUnivers").toInt();
        QString memKey = ipUnivers + "_" + QString::number(numUnivers);

        univers["idUnivers"] = idUniv;
        univers["numeroUnivers"] = numUnivers;

        QString nom = queryUniv.value("nom_univers").toString();
        if (nom.isEmpty()) nom = queryUniv.value("nomUnivers").toString();
        if (nom.isEmpty()) nom = queryUniv.value("nom").toString();
        if (nom.isEmpty()) nom = "Univers " + QString::number(numUnivers);

        univers["nom_univers"] = nom;

        QJsonArray jsonEquips;
        QSqlQuery queryEq;
        queryEq.prepare("SELECT * FROM EQUIPEMENTS WHERE idUnivers = :id");
        queryEq.bindValue(":id", idUniv);

        if (!queryEq.exec()) {
            qDebug() << "❌ ERREUR SQL (Equipements) :" << queryEq.lastError().text();
        }

        while (queryEq.next()) {
            QJsonObject equip;
            int idEq = queryEq.value("idEquipement").toInt();
            if (idEq == 0) idEq = queryEq.value("id").toInt();

            int offsetEquipement = queryEq.value("adresseDepart").toInt();

            equip["idEquipement"] = idEq;

            QString nomEq = queryEq.value("nomEquipement").toString();
            if (nomEq.isEmpty()) nomEq = queryEq.value("nom").toString();
            equip["nomEquipement"] = nomEq;

            equip["adresseDepart"] = offsetEquipement;

            QJsonArray jsonCanaux;
            QSqlQuery queryCanaux;
            queryCanaux.prepare("SELECT * FROM CANAUX WHERE idEquipement = :id");
            queryCanaux.bindValue(":id", idEq);

            if (!queryCanaux.exec()) {
                qDebug() << "❌ ERREUR SQL (Canaux) :" << queryCanaux.lastError().text();
            }

            while (queryCanaux.next()) {
                QJsonObject canal;

                int idCanal = queryCanaux.value("idCanal").toInt();
                if (idCanal == 0) idCanal = queryCanaux.value("id").toInt();

                int numCanal = queryCanaux.value("numeroCanal").toInt();
                canal["numeroCanal"] = numCanal;

                QString chName = queryCanaux.value("description").toString();
                if (chName.isEmpty()) chName = queryCanaux.value("nomCanal").toString();
                if (chName.isEmpty()) chName = "Ch " + QString::number(numCanal);

                canal["chName"] = chName;
                canal["initVal"] = 0;

                int currentVal = 0;
                int indexMemoire = offsetEquipement + numCanal - 2;
                if (m_universState.contains(memKey) && indexMemoire >= 0 && indexMemoire < 512) {
                    currentVal = static_cast<unsigned char>(m_universState[memKey][indexMemoire]);
                }
                canal["valeur"] = currentVal;

                QJsonArray jsonFonc;
                QSqlQuery queryFonc;
                // Attention ici: Je remets FONCTIONNALITE_CANAL comme convenu car c'est le nom dans ta BDD (capture d'écran)
                queryFonc.prepare("SELECT * FROM FONCTIONNALITE_CANAL WHERE idCanal = :id ORDER BY valeurMin ASC");
                queryFonc.bindValue(":id", idCanal);

                if (queryFonc.exec()) {
                    while (queryFonc.next()) {
                        QJsonObject fonc;
                        fonc["valeurMin"] = queryFonc.value("valeurMin").toInt();
                        fonc["valeurMax"] = queryFonc.value("valeurMax").toInt();
                        fonc["fonction"]  = queryFonc.value("fonction").toString();
                        jsonFonc.append(fonc);
                    }
                } else {
                    qDebug() << "❌ ERREUR SQL (Fonctionnalités) :" << queryFonc.lastError().text();
                }

                canal["fonctionnalites"] = jsonFonc;
                jsonCanaux.append(canal);
            }
            equip["channels"] = jsonCanaux;
            jsonEquips.append(equip);
        }
        univers["equipements"] = jsonEquips;
        jsonUniverses.append(univers);
    }

    client->write(QJsonDocument(jsonUniverses).toJson(QJsonDocument::Compact) + "\n");
}

// ========================================================
// TRAITEMENT DES CURSEURS DMX (Art-Net)
// ========================================================

/// @brief Résout l'équipement en BDD, met à jour m_universState et envoie le paquet Art-Net.
void ServeurTcp::envoyerCurseur(int univers_tablette, int idEquipement, int canal, int valeur) {
    Q_UNUSED(univers_tablette);
    QSqlQuery query;

    query.prepare("SELECT U.adresseIp, U.numeroUnivers, E.adresseDepart "
                  "FROM EQUIPEMENTS E "
                  "JOIN UNIVERS U ON E.idUnivers = U.idUnivers "
                  "WHERE E.idEquipement = :id");
    query.bindValue(":id", idEquipement);

    if (!query.exec() || !query.next()) return;

    QString ip = query.value("adresseIp").toString().trimmed();
    int vraiUnivers = query.value("numeroUnivers").toInt();
    int offset = query.value("adresseDepart").toInt();

    QString memKey = ip + "_" + QString::number(vraiUnivers);

    if (!m_universState.contains(memKey)) {
        m_universState[memKey] = QByteArray(512, 0);
    }

    int index = offset + canal - 2;
    if (index >= 0 && index < 512) {
        m_universState[memKey][index] = static_cast<char>(valeur);
        sendArtNetPacket(vraiUnivers, ip, m_universState[memKey]);
    }
}

/// @brief Vérifie l'existence de la scène, détecte un éventuel blackout,
///        puis construit et envoie le paquet Art-Net sur l'univers concerné.
void ServeurTcp::envoyerScene(int idScene)
{
    QSqlQuery queryScene;
    queryScene.prepare("SELECT idScene FROM SCENES WHERE idScene = :id");
    queryScene.bindValue(":id", idScene);
    const bool sceneValide = queryScene.exec() && queryScene.next();

    if (!sceneValide)
    {
        qWarning() << "[envoyerScene] Scène introuvable en base (idScene =" << idScene << ")";
    }
    else
    {
        QSqlQuery queryCount;
        queryCount.prepare("SELECT COUNT(*) FROM PILOTE WHERE idScene = :id");
        queryCount.bindValue(":id", idScene);
        const bool comptageOk = queryCount.exec() && queryCount.next();
        const bool isBlackout = comptageOk && (queryCount.value(0).toInt() == 0);

        if (!comptageOk)
        {
            qWarning() << "[envoyerScene] Erreur comptage PILOTE :" << queryCount.lastError().text();
        }
        else if (isBlackout)
        {
            qDebug() << "[envoyerScene] Scène" << idScene << ": blackout détecté.";

            QSqlQuery queryUnivers;
            queryUnivers.prepare("SELECT numeroUnivers, adresseIp FROM UNIVERS");

            if (!queryUnivers.exec())
            {
                qWarning() << "[envoyerScene] Erreur requête UNIVERS :" << queryUnivers.lastError().text();
            }
            else
            {
                while (queryUnivers.next())
                {
                    const QString ip   = queryUnivers.value("adresseIp").toString().trimmed();
                    const int     univ = queryUnivers.value("numeroUnivers").toInt();
                    const QString key  = ip + "_" + QString::number(univ);

                    m_universState[key] = QByteArray(512, 0);
                    sendArtNetPacket(univ, ip, m_universState[key]);

                    qDebug() << "[envoyerScene] Blackout → univers" << univ << "@ IP" << ip;
                }
            }
        }
        else
        {
            QSqlQuery query;
            query.prepare(
                "SELECT U.numeroUnivers, U.adresseIp,   "
                "       E.adresseDepart, C.numeroCanal, "
                "       P.valeurCanaux                  "
                "FROM PILOTE      P                     "
                "JOIN CANAUX      C ON C.idCanal      = P.idCanal      "
                "JOIN EQUIPEMENTS E ON E.idEquipement = C.idEquipement "
                "JOIN UNIVERS     U ON U.idUnivers    = E.idUnivers    "
                "WHERE P.idScene = :id"
                );
            query.bindValue(":id", idScene);

            if (!query.exec())
            {
                qWarning() << "[envoyerScene] Erreur requête scène active :" << query.lastError().text();
            }
            else
            {
                QSet<QString> clearedKeys;
                QSet<QString> modifiedKeys;

                while (query.next())
                {
                    const QString ip     = query.value("adresseIp").toString().trimmed();
                    const int     univ   = query.value("numeroUnivers").toInt();
                    const int     offset = query.value("adresseDepart").toInt();
                    const int     canal  = query.value("numeroCanal").toInt();
                    const int     val    = query.value("valeurCanaux").toInt();
                    const QString key    = ip + "_" + QString::number(univ);

                    if (!clearedKeys.contains(key))
                    {
                        m_universState[key] = QByteArray(512, 0);
                        clearedKeys.insert(key);
                    }

                    const int index = (offset - 1) + (canal - 1);

                    if (index >= 0 && index < 512)
                    {
                        m_universState[key][index] = static_cast<char>(val);
                        modifiedKeys.insert(key);
                    }
                    else
                    {
                        qWarning() << "[envoyerScene] Index DMX hors limites :" << index
                                   << "(adresseDepart =" << offset
                                   << ", numeroCanal ="  << canal << ")";
                    }
                }

                for (const QString& key : modifiedKeys)
                {
                    const int sep = key.indexOf('_');
                    if (sep != -1)
                    {
                        const QString ip   = key.left(sep);
                        const int     univ = key.mid(sep + 1).toInt();
                        sendArtNetPacket(univ, ip, m_universState[key]);

                        qDebug() << "[envoyerScene] Paquet envoyé → univers" << univ << "@ IP" << ip;
                    }
                }
            }
        }
    }
}

/// @brief Construit la trame ArtDmx v14 et l'émet en UDP sur le port 6454.
void ServeurTcp::sendArtNetPacket(int univers, const QString& ip, const QByteArray& dmxData) {
    QByteArray packet;
    packet.append("Art-Net", 8);
    packet.append((char)0x00);
    packet.append((char)0x50);
    packet.append((char)0x00);
    packet.append((char)0x0E);
    packet.append((char)0x00);
    packet.append((char)0x00);
    packet.append(static_cast<char>(univers & 0xFF));
    packet.append(static_cast<char>((univers >> 8) & 0x7F));
    packet.append((char)0x02);
    packet.append((char)0x00);
    packet.append(dmxData);

    m_udpSocket->writeDatagram(packet, QHostAddress(ip), 6454);
}

/// @brief Retire le socket de m_clients et planifie sa suppression.
void ServeurTcp::onClientDisconnected() {
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket) {
        m_clients.removeAll(socket);
        socket->deleteLater();
        qDebug() << "Client deconnecte.";
    }
}
