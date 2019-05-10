#include <QCoreApplication>
#include "suapp.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    config* configuration = new config();
    new suapp(configuration);

    return a.exec();
}
