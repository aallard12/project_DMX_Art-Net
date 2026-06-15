#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "networkmanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Enregistrement de la classe C++ pour l'exposer au moteur QML
    qmlRegisterType<NetworkManager>("com.dmx.network", 1, 0, "NetworkManager");

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/qt/qml/DMX/Main.qml"_qs);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
