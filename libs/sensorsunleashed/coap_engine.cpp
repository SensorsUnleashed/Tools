#include "coap_engine.h"

bool coap_engine::instanceFlag = false;
coap_engine* coap_engine::coapEngine = nullptr;

coap_engine::coap_engine()
{
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

void coap_engine::addServerInstance(coap_server* ref, quint16 port){
    serverref = ref;
    bindTo(port);
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

void coap_engine::removeZombieTransactions(QHostAddress addr, quint16 port){
    /* Find out if a transaction is ongoing with this particular pdu */
    int done;
    do{
        done = 1;
        for(int i=0; i<transactionList.count(); i++){
            if(transactionList[i]->getPort() == port){
                if(transactionList[i]->getAddr() == addr){
                    delete transactionList[i];
                    transactionList.remove(i);
                    done = 0;
                    break;
                }
            }
        }
    }while(!done);
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
                delete recvPDU;
                return;
            }
        }

        /* Its a new request */
        CoapPDU::Code code = recvPDU->getCode();
        if(code >= CoapPDU::COAP_GET && code <= CoapPDU::COAP_DELETE){
            /* Request - This message is for the server class */
            if(serverref != nullptr){
                if(code == CoapPDU::COAP_GET){
                    serverref->getHandler(recvPDU, addr, port);
                }
                else if(code == CoapPDU::COAP_POST){
                    serverref->postHandler(recvPDU, addr, port);
                }
                else if(code == CoapPDU::COAP_PUT){
                    serverref->putHandler(recvPDU, addr, port);
                }
                else if(code == CoapPDU::COAP_DELETE){
                    serverref->deleteHandler(recvPDU, addr, port);
                }
            }
        }
//        else{
//            delete recvPDU;
//        }
    }
//    else{
//        delete recvPDU;
//    }
    delete recvPDU;
}
