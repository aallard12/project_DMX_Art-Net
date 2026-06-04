#include <QApplication>
#include "chatwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ChatWindow w;
    w.show();
    return app.exec();
}
