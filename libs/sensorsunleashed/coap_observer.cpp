#include "coap_observer.h"
#include <netinet/in.h>

coap_observer::coap_observer(CoapPDU *recvPDU, coap_resource *res, QHostAddress addr, quint16 port)
{
    observer_counter = 0;
    messageid = recvPDU->getMessageID();

    initialPDU = new CoapPDU();
    initialPDU->setCode(recvPDU->getCode());
    initialPDU->setToken(recvPDU->getTokenPointer(), static_cast<uint8_t>(recvPDU->getTokenLength()));
    initialPDU->setURI(res->getUri().data(), static_cast<size_t>(res->getUri().length()));
    initialPDU->setMessageID(messageid);

    this->res = res;
    this->addr = addr;
    this->port = port;
}

CoapPDU* coap_observer::prepare(CoapPDU* response){

    response->setToken(initialPDU->getTokenPointer(), static_cast<uint8_t>(initialPDU->getTokenLength()));
    response->setType(initialPDU->getType());
    response->setMessageID(++messageid);

    uint32_t tmp = htonl(observer_counter);
    response->addOption(CoapPDU::COAP_OPTION_OBSERVE, 4, reinterpret_cast<uint8_t*>(&tmp));
    observer_counter++;

    return initialPDU;
}
