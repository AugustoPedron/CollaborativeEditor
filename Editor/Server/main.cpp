#include <QCoreApplication>
#include "Server/myserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    MyServer s;

    return a.exec();
}
