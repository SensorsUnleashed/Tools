#include "coap_engine.h"

#define DEFAULT_COAP_PORT   5683
bool coap_engine::instanceFlag = false;
coap_engine* coap_engine::coapEngine = nullptr;

coap_engine::coap_engine()
{
    qDebug() << "coap_engine created";
}

coap_engine* coap_engine::getInstance()
{
    if(! instanceFlag)
    {
        coapEngine = new coap_engine();
        instanceFlag = true;
        return coapEngine;
    }
    else
    {
        return coapEngine;
    }
}

void coap_engine::addServerInstance(coap_server* ref){
    serverref = ref;
    bindTo(DEFAULT_COAP_PORT);
}

void coap_engine::rmServerInstance(coap_server* ref){
    Q_UNUSED(ref);
    serverref = nullptr;
    close();
}

void coap_engine::transactionListAdd(coap_transaction* transaction){
    transactionList.append(transaction);
    connect(transaction, SIGNAL(tx_timeout()), this, SLOT(transactionTimeout()));
}

void coap_engine::transactionTimeout(){

    coap_transaction* transaction = dynamic_cast<coap_transaction*>(sender());
    for(int i=0; i<transactionList.count(); i++){
        if(transactionList[i] == transaction){
            delete transactionList[i];
            transactionList.remove(i);
            return;
        }
    }
}

void coap_engine::receive(QHostAddress addr, QByteArray datagram, quint16 port){

    CoapPDU *recvPDU = new CoapPDU(reinterpret_cast<uint8_t*>(datagram.data()),datagram.length());
    if(recvPDU->validate()) {

        /* Find out if a transaction is ongoing with this particular pdu */
        for(int i=0; i<transactionList.count(); i++){
            if(transactionList[i]->checkPDU(addr, recvPDU) == 0){
                if(transactionList[i]->update(recvPDU) == 0){
                    //done remove it.
                    delete transactionList[i];
                    transactionList.remove(i);
                }
                return;
            }
        }

        /* Its a new request */
        CoapPDU::Code code = recvPDU->getCode();
        if(code >= CoapPDU::COAP_GET && code <= CoapPDU::COAP_DELETE){
            /* Request - This message is for the server class */
            if(serverref != nullptr){
                serverref->handleRequest(recvPDU, addr, port);
            }
        }
        else{
            return;
        }
    }
    else{
        delete recvPDU;
    }
}
