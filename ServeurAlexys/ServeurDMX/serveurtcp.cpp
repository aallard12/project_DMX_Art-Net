#include "serveurtcp.h"

ServeurTCP::ServeurTCP(QObject *parent) : QObject(parent)
{
    socketServer = new QTcpServer(this);

    //on connecte l'événement de nouvelle connexion à notre slot
    connect(socketServer, &QTcpServer::newConnection, this, &ServeurTCP::onQTcpServer_newConnection);

    if(!socketServer->listen(QHostAddress::Any,12345)){
        qDebug() << "Le serveur n'a pas démarrer...";
    }else {
        qDebug() << "Serveur démarré sur le port 12345.";
    }
}



void ServeurTCP::onQTcpServer_newConnection()
{

    QTcpSocket *socket = socketServer->nextPendingConnection();

    connect(socket, &QTcpSocket::readyRead, this, &ServeurTCP::onQTcpServer_readyRead);
    connect(socket, &QTcpSocket::disconnected, this, &ServeurTCP::onQTcpServer_disconnected);

    qDebug() << "Nouveau client connecté :" <<socket->peerAddress().toString();

}

void ServeurTCP::onQTcpServer_readyRead()
{
    // Déclaration des variables pour la lecture de la trame
    quint16 taille=0;
    quint8 commande;
    int idScene;


    // Récupération de la socket du client émetteur du signal
    QTcpSocket *client=qobject_cast<QTcpSocket*>(sender());

    // Vérification qu'au moins le champ taille est disponible (2 octets)
    if (client->bytesAvailable() >= (qint64)sizeof(taille))
    {
        // Création du flux de lecture sur la socket
        QDataStream in(client);
        // Lecture de la taille de la trame (en octets)
        qDebug() << "Lecture taille !";
        in >> taille;
        qDebug() << "Taille : " << taille;

        // Vérification que le reste de la trame est complètement arrivé
        if (client->bytesAvailable() >= (qint64)taille)
        {
            // Lecture du caractère de commande
            qDebug() << "Lecture commande !";
            in>>commande;
            qDebug() << "Commande : " << commande;


            // Traitement selon le type de commande
            switch (commande) {
            case 'P':
            {
                qDebug() << "Trame reçue !";
                in>>idScene;
                qDebug() << idScene;
                break;
            }
            }
        }
    }
}

void ServeurTCP::onQTcpServer_disconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    qDebug() << "Client déconnecté :" <<client->peerAddress().toString();
    client->deleteLater();
}
