#ifndef COAP_OBSERVER_H
#define COAP_OBSERVER_H

#include <coap.h>
#include "cantcoap/cantcoap.h"
#include "coap_resource.h"

class coap_observer
{
public:
    coap_observer(CoapPDU *recvPDU, coap_resource* res, QHostAddress addr, quint16 port);

    coap_resource* get(){ return res; }
    CoapPDU* prepare(CoapPDU* response);
    quint16 getPort(){ return port; }
    QHostAddress getAddr(){ return addr; }

private:
    QByteArray uri;
    uint32_t observer_counter;
    uint16_t messageid;
    CoapPDU* initialPDU;
    coap_resource *res;
    QHostAddress addr;
    quint16 port;

};

#endif // COAP_OBSERVER_H
