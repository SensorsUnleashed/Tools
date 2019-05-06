#include "obsserver.h"
#include "suapp.h"
#include <cantcoap/cantcoap.h>
#include <QUrlQuery>
obsserver::obsserver(suapp *parent) : coap_server(0)
{
    nodeinfo* r = new nodeinfo(parent);
    addResource(r);
}

void nodeinfo::handlePUT(CoapPDU *request, CoapPDU *response, QHostAddress addr, quint16 port){
    Q_UNUSED(request);

    QUrlQuery query;

    CoapPDU::CoapOption* options = nullptr;
    options = coap::check_option(request, CoapPDU::COAP_OPTION_URI_QUERY);
    if(options != nullptr){
        QByteArray q(reinterpret_cast<char*>(options->optionValuePointer), options->optionValueLength);
        query.setQuery(q);
    }

    if(query.isEmpty()){
        response->setCode(CoapPDU::COAP_BAD_REQUEST);
    }
    else if(query.hasQueryItem("obs")){
        parent->obsNotifyRestart(addr, port);
        response->setCode(CoapPDU::COAP_CONTENT);
    }
    else{
        response->setCode(CoapPDU::COAP_NOT_FOUND);
    }
}

