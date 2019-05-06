#include <QCoreApplication>

#include "suapp.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    new suapp();

    return a.exec();
}
