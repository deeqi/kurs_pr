#include "myclient.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyClient client("localhost",1234);
    client.show();

    return a.exec();
}
