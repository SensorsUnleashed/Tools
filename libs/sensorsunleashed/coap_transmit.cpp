#include "coap_transaction.h"
#include "coap_engine.h"

coap_transmit::coap_transmit(QHostAddress addr, quint16 port){

    ackTimeout = 1000;
    retransmissions = 3;

    this->addr = addr;
    this->port = port;

    acktimer = new QTimer();
    acktimer->setSingleShot(true);
    connect(acktimer, SIGNAL(timeout()), this, SLOT(timeout()));
}

coap_transmit::~coap_transmit(){
    delete acktimer;
}

/**
  Return 0 if there is no more to do
  Return 1 if we expect something in return
*/
int coap_transmit::transmit(CoapPDU *pdu){

    retransmission_count = 0;
    this->pdu = pdu;

    coap_engine* conn = coap_engine::getInstance();
    conn->send(addr, pdu->getPDUPointer(), pdu->getPDULength(), port);

    if(pdu->getType() == CoapPDU::COAP_CONFIRMABLE){
        acktimer->start(ackTimeout);
        return 1;
    }

    return 0;
}

void coap_transmit::timeout(){
    if(retransmission_count++ >= retransmissions){
        coap_engine* conn = coap_engine::getInstance();
        conn->send(addr, pdu->getPDUPointer(), pdu->getPDULength(), port);
        acktimer->start(ackTimeout*(1<<retransmission_count));
    }
    else{
        notResponding();
        emit tx_timeout();
    }
}

void coap_transmit::done(){
    acktimer->stop();
}
