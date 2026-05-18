/**
 * @file serveurtcp.h
 * @brief Déclaration de la classe ServeurTcp.
 * @version 2.0 (Architecture 100% TCP)
 * @author Alexys LAFFIAC
 * @date 2026
 * @details Ce fichier déclare la classe ServeurTcp qui assure :
 * - L'écoute des connexions TCP entrantes depuis les tablettes de contrôle.
 * - L'envoi des données de configuration (Univers, Scènes) en format JSON.
 * - La résolution des équipements et univers DMX via une base de données MariaDB.
 * - La construction et l'émission de paquets UDP au format Art-Net vers les contrôleurs DMX.
 */
#ifndef SERVEURTCP_H
#define SERVEURTCP_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QSqlDatabase>
#include <QList>
#include <QMap>
#include <QByteArray>

/**
 * @class ServeurTcp
 * @brief Serveur de gestion DMX/Art-Net et API JSON.
 */
class ServeurTcp : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructeur de la classe ServeurTcp.
     * @param port Port d'écoute du serveur TCP (ex : 12345).
     * @param parent Pointeur vers l'objet Qt parent.
     */
    explicit ServeurTcp(quint16 port, QObject *parent = nullptr);

    /**
     * @brief Destructeur de la classe ServeurTcp.
     */
    ~ServeurTcp();

private slots:
    /**
     * @brief Slot appelé lors de l'arrivée d'une nouvelle connexion TCP cliente.
     */
    void onNewConnection();

    /**
     * @brief Slot de réception et d'aiguillage des trames TCP entrantes.
     */
    void onReadyRead();

    /**
     * @brief Slot appelé lors de la déconnexion d'un client TCP.
     */
    void onClientDisconnected();

private:
    /**
     * @brief Initialise et ouvre la connexion à la base de données MariaDB.
     * @return true si la connexion a réussi, false sinon.
     */
    bool setupDatabase();

    /**
     * @brief Rappelle une scène enregistrée en base et applique ses valeurs DMX.
     * @param idScene Identifiant de la scène.
     */
    void envoyerScene(int idScene);

    /**
     * @brief Met à jour l'état mémoire d'un canal DMX et envoie le paquet Art-Net.
     * @param univers_tablette Numéro d'univers transmis par la tablette.
     * @param idEquipement Identifiant de l'équipement.
     * @param canal Numéro de canal relatif à l'équipement.
     * @param valeur Valeur DMX à appliquer (0–255).
     */
    void envoyerCurseur(int univers_tablette, int idEquipement, int canal, int valeur);

    /**
     * @brief Forge et envoie un paquet UDP au format Art-Net (ArtDmx).
     * @param univers Numéro d'univers Art-Net.
     * @param ip Adresse IP de destination.
     * @param dmxData Buffer de 512 octets.
     */
    void sendArtNetPacket(int univers, const QString& ip, const QByteArray& dmxData);

    // ========================================================
    // NOUVELLES FONCTIONS POUR L'API JSON INTEGREE
    // ========================================================

    /**
     * @brief Analyse et route les requêtes JSON de la tablette.
     * @param client Socket TCP du client pour lui répondre.
     * @param json Objet JSON reçu.
     */
    void traiterRequeteJson(QTcpSocket *client, const QJsonObject &json);

    /**
     * @brief Construit et envoie l'arborescence des univers/équipements en JSON.
     * @param client Socket TCP du client.
     */
    void envoyerReponseUniverses(QTcpSocket *client);

    /**
     * @brief Construit et envoie la liste des scènes disponibles en JSON.
     * @param client Socket TCP du client.
     */
    void envoyerReponseScenes(QTcpSocket *client);

    /**
     * @brief Construit et envoie les valeurs DMX d'une scène spécifique en JSON.
     * @param client Socket TCP du client.
     * @param idScene Identifiant de la scène.
     */
    void envoyerReponseSceneValues(QTcpSocket *client, int idScene);

    QTcpServer          *m_tcpServer;   ///< Serveur TCP.
    QUdpSocket          *m_udpSocket;   ///< Socket UDP (Art-Net).
    QSqlDatabase         m_db;          ///< Connexion MariaDB.
    QList<QTcpSocket*>   m_clients;     ///< Liste des clients connectés.
    QMap<QString, QByteArray> m_universState; ///< État mémoire des univers.
};
#endif // SERVEURTCP_H
