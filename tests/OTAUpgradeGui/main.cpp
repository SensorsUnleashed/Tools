#include <QtWidgets/QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QVariant>

#include "su_firmwareupgrade.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("Sensors Unleashed");
    app.setOrganizationDomain("sensorsunleashed.com");
    app.setApplicationName("Upgrade Tool");

    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();

    QHostAddress addr;
    if(argc == 3){
        if(QString(argv[1]).compare("--nodeaddr") == 0){
            addr = QHostAddress(QString(argv[2]));
        }
    }

    if(addr.isNull()){
        qDebug() << "Not a valid IP6 address";
        return 1;
    }

    su_firmwareupgrade* fwupg = new su_firmwareupgrade(addr);
    context->setContextProperty("activenode", fwupg);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
