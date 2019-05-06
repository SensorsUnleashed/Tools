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

    if(code >= CoapPDU::COAP_BAD_REQUEST){
        handleError(token, message, code, ct);
    }
    else{
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

}

void suinterface::handleError(QByteArray token, QByteArray message, CoapPDU::Code code, enum CoapPDU::ContentFormat ct){
    Q_UNUSED(token);
    Q_UNUSED(ct);

    if(code == CoapPDU::COAP_BAD_REQUEST){
        qDebug() << "COAP_BAD_REQUEST";
    }
    else if(code == CoapPDU::COAP_UNAUTHORIZED){
        qDebug() << "COAP_UNAUTHORIZED";

    }
    else if(code == CoapPDU::COAP_BAD_OPTION){
        qDebug() << "COAP_BAD_OPTION";

    }
    else if(code == CoapPDU::COAP_FORBIDDEN){
        qDebug() << "COAP_FORBIDDEN";

    }
    else if(code == CoapPDU::COAP_NOT_FOUND){
        qDebug() << "COAP_NOT_FOUND";

    }
    else if(code == CoapPDU::COAP_METHOD_NOT_ALLOWED){
        qDebug() << "COAP_METHOD_NOT_ALLOWED";

    }
    else if(code == CoapPDU::COAP_NOT_ACCEPTABLE){
        qDebug() << "COAP_NOT_ACCEPTABLE";

    }
    else if(code == CoapPDU::COAP_PRECONDITION_FAILED){
        qDebug() << "COAP_PRECONDITION_FAILED";

    }
    else if(code == CoapPDU::COAP_REQUEST_ENTITY_TOO_LARGE){
        qDebug() << "COAP_REQUEST_ENTITY_TOO_LARGE";

    }
    else if(code == CoapPDU::COAP_UNSUPPORTED_CONTENT_FORMAT){
        qDebug() << "COAP_UNSUPPORTED_CONTENT_FORMAT";

    }
    else if(code == CoapPDU::COAP_INTERNAL_SERVER_ERROR){
        qDebug() << "COAP_INTERNAL_SERVER_ERROR";

    }
    else if(code == CoapPDU::COAP_NOT_IMPLEMENTED){
        qDebug() << "COAP_NOT_IMPLEMENTED";

    }
    else if(code == CoapPDU::COAP_BAD_GATEWAY){
        qDebug() << "COAP_BAD_GATEWAY";

    }
    else if(code == CoapPDU::COAP_SERVICE_UNAVAILABLE){
        qDebug() << "COAP_SERVICE_UNAVAILABLE";

    }
    else if(code == CoapPDU::COAP_GATEWAY_TIMEOUT){
        qDebug() << "COAP_GATEWAY_TIMEOUT";

    }
    else if(code == CoapPDU::COAP_PROXYING_NOT_SUPPORTED){
        qDebug() << "COAP_PROXYING_NOT_SUPPORTED";

    }
    else{
        qDebug() << "COAP_UNDEFINED_CODE";
    }

    if(message.length()) qDebug() << message;
}


#include "QHash"
int suinterface::getTokenref(QByteArray token){
    QHash<QByteArray, int>::iterator i = tokenref.find(token);
    return i.value();
}

void suinterface::setTokenref(QByteArray token, int ref){
    tokenref.insert(token, ref);
}
