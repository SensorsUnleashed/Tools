#include "coap_server.h"

#include "coap_engine.h"
#include <string.h>

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

coap_transaction* coap_server::handleRequest(CoapPDU *recvPDU, QHostAddress addr, quint16 port){

    CoapPDU* response = new CoapPDU;
    CoapPDU::Code code = recvPDU->getCode();
    QByteArray payload;

    response->setCode(recvPDU->getCode());
    response->setToken(recvPDU->getTokenPointer(), recvPDU->getTokenLength());
    response->setType(recvPDU->getType());
    response->setMessageID(recvPDU->getMessageID()+1);

    coap_resource* res = nullptr;
    for(int i=0; i<resources.count(); i++){
        char dst[50] = {0};
        int outLen;
        if(recvPDU->getURI(dst, 50, &outLen) == 0){
            char* s = resources[i]->getUri().data();
            if(strcmp(s, dst) == 0){
                res = resources[i];
                break;
            }
        }
    }

    if(res){
        if(code == CoapPDU::COAP_GET){
            res->handleGET(recvPDU, response, payload);
        }
        else if(code == CoapPDU::COAP_POST){
            res->handlePOST(recvPDU, response);
        }
        else if(code == CoapPDU::COAP_PUT){
            res->handlePUT(recvPDU, response);
        }
        else if(code == CoapPDU::COAP_DELETE){
            res->handleDELETE(recvPDU, response);
        }
        else{

        }
    }
    else{   //Unknown resource
        if(code == CoapPDU::COAP_GET){
            handleGET(recvPDU, response, payload);
        }
        else if(code == CoapPDU::COAP_POST){
            handlePOST(recvPDU, response);
        }
        else if(code == CoapPDU::COAP_PUT){
            handlePUT(recvPDU, response);
        }
        else if(code == CoapPDU::COAP_DELETE){
            handleDELETE(recvPDU, response);
        }
        else{

        }
    }

    return new coap_transaction(addr, port, response, nullptr, payload);
}


void coap_server::handleGET(CoapPDU *request, CoapPDU *response, QByteArray payload){
    Q_UNUSED(request);
    Q_UNUSED(response);
    Q_UNUSED(payload);

    //    pdu->setType(CoapPDU::COAP_CONFIRMABLE);
    //    pdu->setCode(CoapPDU::COAP_GET);
    //    pdu->setToken((uint8_t*)&t.number,2);

    //    enum CoapPDU::ContentFormat ct = CoapPDU::COAP_CONTENT_FORMAT_APP_OCTET;
    //    pdu->addOption(CoapPDU::COAP_OPTION_CONTENT_FORMAT,1,(uint8_t*)&ct);
    //    pdu->setMessageID(t.number);

    //    char tmp[30];
    //    int len;
    //    pdu->getURI(tmp,30, &len);

    //    coap_engine* en = coap_engine::getInstance();
    //    en->transactionListAdd(new coap_transaction(addr, pdu, this, payload));
    //    //send(pdu, t, payload, allow_retry);


    qDebug() << "Implement handleGET";
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

    /* Each entry in the this entity manager is named /RM1, /RM2, /RM3 etc..*/
    int i = 1;
    while(1){
        coap_resource* r = parent->getResource(i);
        if(r){
            QString str = "<" + r->getUri() + ">";
            i++;
            payload->append(str);
        }
        else{
            break;
        }
    }

    enum CoapPDU::ContentFormat ct = CoapPDU::COAP_CONTENT_FORMAT_APP_LINK;
    response->addOption(CoapPDU::COAP_OPTION_CONTENT_FORMAT,1,reinterpret_cast<uint8_t*>(&ct));

}

