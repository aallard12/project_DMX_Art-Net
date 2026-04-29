#include "interfacepcclient.h"
#include "accessbdd.h"

#include <QApplication>
#include <QTest>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    InterfacePcClient w;
    w.setWindowTitle("DMX Equipment Manager");
    w.showMaximized();

    return app.exec();
}
