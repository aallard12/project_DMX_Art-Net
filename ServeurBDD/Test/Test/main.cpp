#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    // On lance le serveur sur le port 1234
    MyServer server(12345);

    return a.exec();
}
