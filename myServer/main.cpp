#include "myserver.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    myServer server(1234);
    server.show();

    return a.exec();
}
