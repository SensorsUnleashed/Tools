#include "suapp.h"

suapp::suapp()
{
    node* n1 = new node(QHostAddress("fd00::212:4b00:18d6:f180"));
    coap_resource* r1 = new coap_resource();
    r1->setUri("su/utoc");
    s1 = new pulsecounter(n1, r1);

    connect(s1, SIGNAL(currentValueChanged(quint16, QVariant)), this, SLOT(valueUpdate(quint16, QVariant)));
    s1->requestValue(command);
}

void suapp::eventSetupRdy(){

}

void suapp::valueUpdate(quint16 token, QVariant result){
    Q_UNUSED(token);
    qDebug() << result;
}
