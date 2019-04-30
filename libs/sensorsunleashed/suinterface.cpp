#include "node.h"
#include <QRandomGenerator>
#include "su_message.h"
#include "coap_engine.h"

suinterface::suinterface(QHostAddress addr, quint16 port){
    this->addr = addr;
    this->port = port;
}

QByteArray suinterface::get_request(CoapPDU *pdu, int req, QByteArray payload, quint8 allow_retry){
    Q_UNUSED(allow_retry);
    quint32 t = QRandomGenerator::global()->generate();

    QByteArray token;

    token.append(static_cast<char>(t));
    token.append(static_cast<char>(t >> 8));
    token.append(static_cast<char>(t >> 16));
    token.append(static_cast<char>(t >> 24));

    pdu->setType(CoapPDU::COAP_CONFIRMABLE);
    pdu->setCode(CoapPDU::COAP_GET);
    pdu->setToken(reinterpret_cast<uint8_t*>(token.data()), static_cast<uint8_t>(token.length()));

    enum CoapPDU::ContentFormat ct = CoapPDU::COAP_CONTENT_FORMAT_APP_OCTET;
    pdu->addOption(CoapPDU::COAP_OPTION_CONTENT_FORMAT,1,reinterpret_cast<uint8_t*>(&ct));
    pdu->setMessageID(1);

    new coap_client_transaction(addr, port, pdu, this, payload);

    setTokenref(token, req);

    return token;
}

QByteArray suinterface::put_request(CoapPDU *pdu, int req, QByteArray payload, quint8 allow_retry){
    Q_UNUSED(allow_retry);
    quint32 t = QRandomGenerator::global()->generate();

    QByteArray token;

    token.append(static_cast<char>(t));
    token.append(static_cast<char>(t >> 8));
    token.append(static_cast<char>(t >> 16));
    token.append(static_cast<char>(t >> 24));

    pdu->setType(CoapPDU::COAP_CONFIRMABLE);
    pdu->setCode(CoapPDU::COAP_PUT);
    pdu->setToken(reinterpret_cast<uint8_t*>(token.data()), static_cast<uint8_t>(token.length()));

    enum CoapPDU::ContentFormat ct = CoapPDU::COAP_CONTENT_FORMAT_APP_OCTET;
    pdu->addOption(CoapPDU::COAP_OPTION_CONTENT_FORMAT,1,reinterpret_cast<uint8_t*>(&ct));
    pdu->setMessageID(1);

    new coap_client_transaction(addr, port, pdu, this, payload);

    setTokenref(token, req);

    return token;
}

QByteArray suinterface::request(CoapPDU *pdu, int req, QByteArray payload){

    quint32 t = QRandomGenerator::global()->generate();

    QByteArray token;

    token.append(static_cast<char>(t));
    token.append(static_cast<char>(t >> 8));
    token.append(static_cast<char>(t >> 16));
    token.append(static_cast<char>(t >> 24));

    pdu->setToken(reinterpret_cast<uint8_t*>(token.data()), static_cast<uint8_t>(token.length()));

    new coap_client_transaction(addr, port, pdu, this, payload);

    setTokenref(token, req);

    return token;
}

//Returns an acknowledment, if that is needed
void suinterface::parseMessage(QByteArray token, QByteArray message, CoapPDU::Code code, enum CoapPDU::ContentFormat ct){

    switch(ct){
    case CoapPDU::COAP_CONTENT_FORMAT_TEXT_PLAIN:
        parseTextPlainFormat(token, message);
        break;
    case CoapPDU::COAP_CONTENT_FORMAT_APP_LINK:
        parseAppLinkFormat(token, message);
        break;
    case CoapPDU::COAP_CONTENT_FORMAT_APP_XML:
        parseAppXmlFormat(token, message);
        qDebug() << "CoapPDU::COAP_CONTENT_FORMAT_APP_XML";
        break;
    case CoapPDU::COAP_CONTENT_FORMAT_APP_OCTET:
        parseAppOctetFormat(token, message, code);
        break;
    case CoapPDU::COAP_CONTENT_FORMAT_APP_EXI:
        parseAppExiFormat(token, message);
        qDebug() << "CoapPDU::COAP_CONTENT_FORMAT_APP_EXI";
        break;
    case CoapPDU::COAP_CONTENT_FORMAT_APP_JSON:
        parseAppJSonFormat(token, message);
        qDebug() << "CoapPDU::COAP_CONTENT_FORMAT_APP_JSON";
        break;
    }
}
#include "QHash"
int suinterface::getTokenref(QByteArray token){
    QHash<QByteArray, int>::iterator i = tokenref.find(token);
    return i.value();
}

void suinterface::setTokenref(QByteArray token, int ref){
    tokenref.insert(token, ref);
}
