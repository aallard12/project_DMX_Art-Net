#include <QApplication>
#include "interfacepcclient.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowTitle("DMX Equipment Manager");
    w.show();

    return a.exec();
}
