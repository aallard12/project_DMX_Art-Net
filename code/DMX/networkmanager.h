/**
 * @file networkmanager.h
 * @brief En-tête de la classe NetworkManager.
 * @author Valentin
 * @date 2026
 * @details Gère la communication réseau TCP de manière asynchrone entre l'interface
 * QML (Tablette) et le serveur DMX (Raspberry Pi).
 */

// 💡 [EXPLICATION] : Ces deux lignes (#ifndef et #define) sont des "Include Guards" (Gardiens d'inclusion).
// Elles empêchent le compilateur d'inclure ce fichier deux fois par erreur, ce qui ferait planter la compilation.
#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

// 💡 [EXPLICATION] : Importation des bibliothèques Qt nécessaires pour construire la classe.
#include <QObject>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantList>
#include <QBuffer>
#include <QDataStream>
#include <QByteArray>

/**
 * @class NetworkManager
 * @brief Classe responsable des échanges réseau avec le serveur DMX.
 * @details Fait le pont entre l'interface QML et le serveur TCP. Gère l'envoi
 * des commandes et la réception continue des données JSON ou binaires.
 */
// 💡 [EXPLICATION] : ": public QObject" signifie que ta classe hérite des super-pouvoirs de Qt.
// C'est obligatoire pour pouvoir utiliser les Signaux, les Slots, et communiquer avec le QML.
class NetworkManager : public QObject {
    // 💡 [EXPLICATION] : Q_OBJECT est une macro obligatoire dès qu'on hérite de QObject.
    // Elle indique au compilateur Qt (le MOC) de générer le code caché pour gérer les signaux et slots.
    Q_OBJECT

    /**
     * @property isConnected
     * @brief Indique l'état actuel de la connexion TCP. Exposée au QML.
     */
    // 💡 [EXPLICATION] : Q_PROPERTY crée une variable visible par le QML.
    // - READ indique quelle fonction C++ appeler quand le QML demande sa valeur.
    // - NOTIFY indique quel signal alertera le QML que la valeur a changé (pour qu'il se rafraîchisse).
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionChanged)

public:
    /**
     * @brief Constructeur de la classe NetworkManager.
     * @param parent Objet parent QObject.
     */
    explicit NetworkManager(QObject *parent = nullptr);

    /**
     * @brief Vérifie si la socket est actuellement connectée au serveur.
     * @return true si connecté, false sinon.
     */
    bool isConnected();

    // 💡 [EXPLICATION] : Q_INVOKABLE est la "clé magique". Si tu ne mets pas ça,
    // le Javascript/QML de ton interface ne pourra jamais appeler ces fonctions.
    // Elles resteraient bloquées du côté C++.

    /**
     * @brief Demande au serveur l'arborescence complète des univers et équipements.
     */
    Q_INVOKABLE void fetchUniverses();

    /**
     * @brief Demande au serveur la liste des scènes disponibles.
     */
    Q_INVOKABLE void fetchScenes(int idUnivers = -1);

    /**
     * @brief Demande au serveur les valeurs des canaux associées à une scène.
     * @param idScene Identifiant de la scène à récupérer.
     */
    Q_INVOKABLE void fetchSceneValues(int idScene);

    /**
     * @brief Établit une connexion TCP avec le serveur DMX.
     * @param ip Adresse IP du serveur.
     * @param port Port d'écoute du serveur.
     */
    Q_INVOKABLE void connectToTcpServer(const QString &ip, int port);

    /**
     * @brief Demande au serveur le lancement d'une scène spécifique.
     * @param idScene Identifiant de la scène à lancer.
     */
    Q_INVOKABLE void lancerSceneTCP(int idScene);

    /**
     * @brief Envoie une trame binaire au serveur pour modifier la valeur d'un canal DMX.
     * @param univers Numéro de l'univers DMX.
     * @param projecteur Identifiant du projecteur cible.
     * @param canal Numéro du canal DMX à modifier.
     * @param valeur Nouvelle valeur DMX (0-255).
     */
    Q_INVOKABLE void setDmxValueTCP(int univers, int projecteur, int canal, int valeur);

    /**
     * @brief Envoie une demande d'authentification au serveur.
     * @param username Nom d'utilisateur saisi.
     * @param password Mot de passe saisi en clair.
     */
    Q_INVOKABLE void sendLoginTCP(const QString &username, const QString &password);

signals:
    // 💡 [EXPLICATION] : La section "signals" contient les événements que ta classe C++ peut déclencher ("émettre").
    // Ces fonctions n'ont pas de corps (pas de code dans le .cpp), elles servent juste de "cri" que le QML va entendre.

    /**
     * @brief Signal émis lorsque l'état de la connexion change.
     */
    void connectionChanged();

    /**
     * @brief Signal émis lors de la réception de l'arborescence des univers.
     * @param data Liste formatée pour le moteur QML.
     */
    // 💡 [EXPLICATION] : Le type QVariantList est très important ici. C'est le seul format de tableau (array)
    // que le C++ et le QML/JS comprennent tous les deux sans faire de conversion compliquée.
    void universesReceived(QVariantList data);

    /**
     * @brief Signal émis lors de la réception de la liste des scènes.
     * @param data Liste formatée pour le moteur QML.
     */
    void scenesReceived(QVariantList data);

    /**
     * @brief Signal émis lors de la réception des valeurs d'une scène.
     * @param data Liste formatée pour le moteur QML.
     */
    void sceneValuesReceived(QVariantList data);

    /**
     * @brief Signal émis lors de la réception de la réponse d'authentification.
     * @param success true si les identifiants sont corrects, false sinon.
     */
    void loginResponseReceived(bool success);

private slots:
    // 💡 [EXPLICATION] : "private slots" signifie que ces fonctions réagissent à des événements (signaux),
    // mais qu'elles sont strictement réservées à l'usage interne de cette classe.

    /**
     * @brief Slot déclenché automatiquement lors de la réception de données TCP.
     * @details Concatène les données reçues et extrait les paquets complets délimités par le caractère de nouvelle ligne.
     */
    void onTcpReadyRead();

private:
    // 💡 [EXPLICATION] : Les membres privés sont invisibles de l'extérieur. L'interface QML ne sait
    // même pas que `tcpSocket` existe. C'est ce qu'on appelle l'"Encapsulation" en programmation orientée objet.
    QTcpSocket tcpSocket;   ///< Socket TCP pour la communication avec le serveur.
    QByteArray tcpBuffer;   ///< Tampon de réception pour gérer la fragmentation des paquets.
};

#endif
