#include "node.h"

suinterface::suinterface(QHostAddress addr) : wsn(addr){



}

quint16 suinterface::get_request(CoapPDU *pdu, enum request req, QByteArray payload, quint8 allow_retry){
    msgid t;
    t.req = req;
    t.number = qrand();

    pdu->setType(CoapPDU::COAP_CONFIRMABLE);
    pdu->setCode(CoapPDU::COAP_GET);
    pdu->setToken((uint8_t*)&t.number,2);

    enum CoapPDU::ContentFormat ct = CoapPDU::COAP_CONTENT_FORMAT_APP_OCTET;
    pdu->addOption(CoapPDU::COAP_OPTION_CONTENT_FORMAT,1,(uint8_t*)&ct);
    pdu->setMessageID(t.number);

    char tmp[30];
    int len;
    pdu->getURI(tmp,30, &len);

    send(pdu, t, payload, allow_retry);

    return t.number;
}

quint16 suinterface::put_request(CoapPDU *pdu, enum request req, QByteArray payload, quint8 allow_retry){
    msgid t;
    t.req = req;
    t.number = qrand();

    pdu->setType(CoapPDU::COAP_CONFIRMABLE);
    pdu->setCode(CoapPDU::COAP_PUT);

    //Set the token, if it is not set beforehand
    if(pdu->getTokenLength() <= 0){
        pdu->setToken((uint8_t*)&t.number,2);
    }

    enum CoapPDU::ContentFormat ct = CoapPDU::COAP_CONTENT_FORMAT_APP_OCTET;
    pdu->addOption(CoapPDU::COAP_OPTION_CONTENT_FORMAT,1,(uint8_t*)&ct);
    pdu->setMessageID(t.number);

    send(pdu, t, payload, allow_retry);

    return t.number;
}
