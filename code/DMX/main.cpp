/**
 * @file main.cpp
 * @brief Point d'entrée de l'application Tablette DMX.
 * @author Valentin
 * @date 2026
 * @details Ce fichier initialise le moteur QML, instancie la fenêtre principale
 * et déclare la classe NetworkManager pour qu'elle soit utilisable côté interface.
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "networkmanager.h"

/**
 * @brief Fonction principale de l'application.
 * @param argc Nombre d'arguments en ligne de commande.
 * @param argv Tableau des arguments en ligne de commande.
 * @return Code de retour de l'application (0 si succès).
 */
int main(int argc, char *argv[])
{
    // 💡 [EXPLICATION] : Initialise l'application graphique. C'est le composant central
    // qui gère la souris, l'écran tactile et la boucle d'événements du système d'exploitation.
    QGuiApplication app(argc, argv);

    // Enregistrement de la classe C++ pour l'exposer au moteur QML
    // 💡 [EXPLICATION] : Étape vitale de ton architecture ! On indique au moteur QML
    // que ta classe C++ "NetworkManager" peut être utilisée comme un composant QML classique.
    // C'est ce qui te permet de faire l'import "com.dmx.network 1.0" dans ton Main.qml.
    qmlRegisterType<NetworkManager>("com.dmx.network", 1, 0, "NetworkManager");

    // 💡 [EXPLICATION] : Instanciation du moteur QML. C'est lui qui va lire tes fichiers .qml,
    // interpréter le Javascript qu'ils contiennent, et les dessiner à l'écran.
    QQmlApplicationEngine engine;

    // 💡 [EXPLICATION] : Chemin d'accès vers ton fichier racine. "qrc:/" signifie que le fichier
    // est compilé directement à l'intérieur de ton exécutable (ou APK) en tant que ressource.
    const QUrl url(u"qrc:/qt/qml/DMX/Main.qml"_qs);

    // 💡 [EXPLICATION] : Connexion de sécurité (Watchdog de démarrage).
    // On écoute le signal émis par le moteur quand il essaie de construire ton interface.
    // Si l'objet n'a pas pu être créé (ex: grosse erreur de syntaxe dans le QML),
    // on ferme l'application de force avec le code d'erreur -1 pour éviter un écran noir bloquant.
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    // 💡 [EXPLICATION] : Ordre donné au moteur de charger et d'afficher le fichier Main.qml.
    engine.load(url);

    // 💡 [EXPLICATION] : Lancement de la boucle infinie de l'application (Event Loop).
    // Le programme va rester bloqué sur cette ligne indéfiniment, à écouter
    // tes appuis sur l'écran tactile et les arrivées de paquets TCP sur le réseau.
    return app.exec();
}
