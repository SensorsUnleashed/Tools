#include "suapp.h"
#include "woody.h"
suapp::suapp(QQmlContext *context)
{
    w = new woody;

    node* n1 = new node(QHostAddress("fd00::212:4b00:18d6:f180"));
    s1 = new pulsecounter(n1, "su/utoc", QVariantMap());

    connect(s1, SIGNAL(currentValueChanged(quint16, QVariant)), this, SLOT(valueUpdate(quint16, QVariant)));
    s1->requestValue(command);


    context->setContextProperty("kettle", w);


    polltimer = new QTimer();
    connect(polltimer, SIGNAL(timeout()), this, SLOT(poll()));
    polltimer->setInterval(10*1000);
    polltimer->setSingleShot(true);
}

void suapp::poll(){
    s1->requestValue(command);
}

void suapp::valueUpdate(quint16 token, QVariant result){
    //qDebug() << result;
    polltimer->start();

    w->setOperatingdata(result.toMap()["value"].toString());
    qDebug() << w->reading("tempKettle").toDouble() << "C " <<  w->reading("power").toDouble() << " (" << w->reading("dutyCycle").toInt() << ")";
    qDebug() << w->reading("capacity").toDouble() << " " << w->reading("feederTimeSec").toInt();
}
