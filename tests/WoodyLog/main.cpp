#include <QCoreApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QVariant>

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "suapp.h"


int main(int argc, char *argv[])
{
     QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

     QGuiApplication app(argc, argv);
     app.setOrganizationName("Woody pillebrænder");
     app.setOrganizationDomain("sensorsunleashed.com");
     app.setApplicationName("Monitor");

     QQmlApplicationEngine engine;
     QQmlContext *context = engine.rootContext();

     suapp* su = new suapp(context);

     //QQmlApplicationEngine engine;
     engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
     if (engine.rootObjects().isEmpty())
         return -1;

    return app.exec();
}
