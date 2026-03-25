#include <QCoreApplication>
#include "serveurtcp.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ServeurTCP server;

    return a.exec();
}
