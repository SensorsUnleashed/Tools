#include "suapp.h"

suapp::suapp()
{
    node* n1 = new node(QHostAddress("fd00::212:4b00:18d6:f180"));
    s1 = new pulsecounter(n1, "su/utoc", QVariantMap());

    connect(s1, SIGNAL(currentValueChanged(quint16, QVariant)), this, SLOT(valueUpdate(quint16, QVariant)));
    s1->requestValue(command);
}

void suapp::eventSetupRdy(){

}

void suapp::valueUpdate(quint16 token, QVariant result){
    qDebug() << result;
}
