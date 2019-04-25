#include "coap_server.h"

#include "coap_engine.h"
#include "coap_observer.h"
#include <string.h>
#include <QUrl>
#include <QUrlQuery>
#include <coap.h>

coap_server::coap_server()
{
    coap_engine* en = coap_engine::getInstance();
    core = new res_core_well_known(this);
    addResource(core);
    en->addServerInstance(this);
}

coap_server::~coap_server(){
    coap_engine* en = coap_engine::getInstance();
    en->rmServerInstance(this);
}

void coap_server::addResource(coap_resource *resource){
    resources.append(resource);
}

coap_resource* coap_server::getResource(int index){
    coap_resource* r = nullptr;
    if(resources.size() > index){
        r = resources.at(index);
    }

    return r;
}

coap_transaction* coap_server::getHandler(CoapPDU *recvPDU, QHostAddress addr, quint16 port){
    CoapPDU* response = new CoapPDU;
    CoapPDU::Code code = recvPDU->getCode();
    QByteArray payload;
    int observe = -1;

    CoapPDU::CoapOption* options = coap::check_option(recvPDU, CoapPDU::COAP_OPTION_OBSERVE);
    if(options != nullptr){
        observe = 0;    //To avoid problems where optionValueLength is less then sizeof(int)
        memcpy(&observe, options->optionValuePointer, options->optionValueLength);
    }

    response->setCode(code);
    response->setToken(recvPDU->getTokenPointer(), static_cast<uint8_t>(recvPDU->getTokenLength()));
    response->setType(recvPDU->getType());
    response->setMessageID(recvPDU->getMessageID()+1);

    coap_resource* res = findResource(recvPDU);
    if(res){
        if(observe == 0){    //Register observer
            remove_observer_by_uri(res, addr, port);
            coap_observer* c = new coap_observer(recvPDU, res, addr, port);
            c->prepare(response);
            observers.append(c);
        }
        else if(observe == 1){ //De-register observer
            remove_observer_by_uri(res, addr, port);
        }
        res->handleGET(recvPDU, response, &payload);
    }
    else{   //Unknown resource
    }
    delete recvPDU;
    return new coap_server_transaction(addr, port, response, this, *&payload);
}

coap_transaction* coap_server::postHandler(CoapPDU *recvPDU, QHostAddress addr, quint16 port){
    CoapPDU* response = new CoapPDU;
    QByteArray payload;
    coap_resource* res = findResource(recvPDU);
    if(res){
        res->handlePOST(recvPDU, response);
    }
    delete recvPDU;
    return new coap_server_transaction(addr, port, response, this, *&payload);
}
coap_transaction* coap_server::putHandler(CoapPDU *recvPDU, QHostAddress addr, quint16 port){
    CoapPDU* response = new CoapPDU;
    QByteArray payload;
    coap_resource* res = findResource(recvPDU);
    if(res){
        res->handlePUT(recvPDU, response);
    }
    delete recvPDU;
    return new coap_server_transaction(addr, port, response, this, *&payload);
}
coap_transaction* coap_server::deleteHandler(CoapPDU *recvPDU, QHostAddress addr, quint16 port){
    CoapPDU* response = new CoapPDU;
    QByteArray payload;
    coap_resource* res = findResource(recvPDU);
    if(res){
        res->handleDELETE(recvPDU, response);
    }
    delete recvPDU;
    return new coap_server_transaction(addr, port, response, this, *&payload);
}

coap_resource* coap_server::findResource(CoapPDU *PDU){
    char dst[50] = {0};
    int outLen;
    coap_resource* res = nullptr;
    if(PDU->getURI(dst, 50, &outLen) == 0){
        QUrl reqUrl(dst);
        for(int i=0; i<resources.count(); i++){
            QUrl resUrl(resources[i]->getUri());
            if(reqUrl.path() == resUrl.path()){
                res = resources[i];
                break;
            }
        }
    }
    return res;
}

void coap_server::receiverNotResponding(QHostAddress addr, quint16 port){
    remove_observer_by_addr(addr, port);
}

/* Remove all observers, no matter which */
void coap_server::remove_observer_by_addr(QHostAddress addr, quint16 port){
    int found;

    do{
        found = 0;
        for(int i=0; i<observers.count(); i++){
            coap_observer* o = observers[i];
            if(o->getAddr() == addr && o->getPort() == port) {
                observers.removeAt(i);
                qDebug() << "Remove " << addr;
                delete o;
                found = 1;
                break;
            }
        }
    }while(found);

}

void coap_server::remove_observer_by_uri(coap_resource* res, QHostAddress addr, quint16 port){
    for(int i=0; i<observers.count(); i++){
        coap_observer* o = observers[i];
        if(o->get() == res && o->getAddr() == addr && o->getPort() == port) {
            observers.removeAt(i);
            delete o;
            break;
        }
    }
}

void coap_server::handleObservers(coap_resource* res){
    /* Find out if anyone is observing the resource */
    for(int i=0; i<observers.count(); i++){
        coap_observer* o = observers[i];
        if(o->get() == res){
            CoapPDU* response = new CoapPDU();
            CoapPDU* recvPDU = o->prepare(response);
            QByteArray payload;
            res->handleGET(recvPDU, response, &payload);
            new coap_server_transaction(o->getAddr(), o->getPort(), response, this, *&payload);
        }
    }
}

void coap_server::handleGET(CoapPDU *request, CoapPDU *response, QByteArray *payload){
    Q_UNUSED(request);
    enum CoapPDU::ContentFormat ct = CoapPDU::COAP_CONTENT_FORMAT_TEXT_PLAIN;
    response->setCode(CoapPDU::COAP_BAD_REQUEST);
    response->addOption(CoapPDU::COAP_OPTION_CONTENT_FORMAT,1,reinterpret_cast<uint8_t*>(&ct));
    payload->append("Resource unknown");
}

void coap_server::handlePOST(CoapPDU *request, CoapPDU *response){
    Q_UNUSED(request);
    Q_UNUSED(response);
    qDebug() << "Implement handlePOST";
}

void coap_server::handlePUT(CoapPDU *request, CoapPDU *response){
    Q_UNUSED(request);
    Q_UNUSED(response);
    qDebug() << "Implement handlePUT";
}

void coap_server::handleDELETE(CoapPDU *request, CoapPDU *response){
    Q_UNUSED(request);
    Q_UNUSED(response);
    qDebug() << "Implement handleDELETE";
}

res_core_well_known::res_core_well_known(coap_server* parent){
    this->parent = parent;
}

void res_core_well_known::handleGET(CoapPDU *request, CoapPDU *response, QByteArray* payload){
    Q_UNUSED(request);

    qDebug() << "coap_server::res_core_well_known::handleGET";

    int i = 0;
    while(1){
        coap_resource* r = parent->getResource(i);
        if(r){
            if(r != this){
                QString str = "<" + r->getUri() + ">";
                payload->append(str);
            }
        }
        else{
            break;
        }
        i++;
    }

    enum CoapPDU::ContentFormat ct = CoapPDU::COAP_CONTENT_FORMAT_APP_LINK;
    response->addOption(CoapPDU::COAP_OPTION_CONTENT_FORMAT,1,reinterpret_cast<uint8_t*>(&ct));
}

