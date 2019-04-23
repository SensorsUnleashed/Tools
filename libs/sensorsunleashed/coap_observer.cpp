#include "coap_observer.h"

coap_observer::coap_observer(CoapPDU *recvPDU, coap_resource *res, QHostAddress addr, quint16 port)
{
    memcpy(&this->recvPDU, recvPDU, sizeof(CoapPDU));
    this->res = res;
    this->addr = addr;
    this->port = port;
    observer_counter = 0;
}

void coap_observer::prepare(CoapPDU* recvPDU, CoapPDU* response){

    memcpy(recvPDU, &this->recvPDU, sizeof(CoapPDU));

    response->setCode(recvPDU->getCode());
    response->setToken(recvPDU->getTokenPointer(), static_cast<uint8_t>(recvPDU->getTokenLength()));
    response->setType(recvPDU->getType());
    response->setMessageID(1);
    response->addOption(CoapPDU::COAP_OPTION_OBSERVE, 4, reinterpret_cast<uint8_t*>(&observer_counter));
    observer_counter++;
}
