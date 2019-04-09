#include "rdserver.h"
#include <QDebug>
#include <string.h>


rdServer::rdServer()
{
    coap_server* server = new coap_server();

    core = new rd_res_core_well_known(this);

    server->addResource(this);
    //server->addResource(core);
}

void rdServer::addNode(node *n){
    nodes.append(n);
}

node* rdServer::getNodeRef(int index){
    node* n = nullptr;
    if(nodes.size() > index){
        n = nodes.at(index);
    }
    return n;
}

void rdServer::handleGET(CoapPDU *request, CoapPDU *response, QByteArray* payload){
    Q_UNUSED(response);
    Q_UNUSED(request);
    Q_UNUSED(payload);
    qDebug() << "rdServer::handleGET";
}

void rdServer::handlePOST(CoapPDU *request, CoapPDU *response){
    Q_UNUSED(request);
    Q_UNUSED(response);
}

void rdServer::handlePUT(CoapPDU *request, CoapPDU *response){
    Q_UNUSED(request);
    Q_UNUSED(response);
}

void rdServer::handleDELETE(CoapPDU *request, CoapPDU *response){
    Q_UNUSED(request);
    Q_UNUSED(response);
}


rd_res_core_well_known::rd_res_core_well_known(rdServer* parent){
    this->parent = parent;
}

void rd_res_core_well_known::handleGET(CoapPDU *request, CoapPDU *response, QByteArray* payload){
    Q_UNUSED(request);

    qDebug() << "rdServer::res_core_well_known::handleGET";

    /* Each entry in the this entity manager is named /RM1, /RM2, /RM3 etc..*/
    int i = 1;
    while(1){
        node* n = parent->getNodeRef(i);
        if(n){
            QString str = "</RM" + QString::number(i, 10) + ">";
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

