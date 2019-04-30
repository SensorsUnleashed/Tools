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

    cleanuptimer = new QTimer();
    cleanuptimer->setSingleShot(true);
    connect(cleanuptimer, SIGNAL(timeout()), this, SLOT(cleanupTimeout()));
}

coap_transmit::~coap_transmit(){
     qDebug() << "delete";
    delete acktimer;
    delete cleanuptimer;
}

/**
  Return 0 if there is no more to do
  Return 1 if we expect something in return
*/
int coap_transmit::transmitHandler(CoapPDU *pdu){
    int ret = 0;
    retransmission_count = 0;
    this->pdu = pdu;

    transmitOnly(pdu);

    if(pdu->getType() == CoapPDU::COAP_ACKNOWLEDGEMENT){
        cleanuptimer->start(15000);
        ret = 1;
    }
    else if(pdu->getType() == CoapPDU::COAP_CONFIRMABLE){
        acktimer->start(ackTimeout);
        ret = 1;
    }

    return ret;
}

void coap_transmit::transmitOnly(CoapPDU* pdu){
    coap_engine* conn = coap_engine::getInstance();
    conn->send(addr, pdu->getPDUPointer(), pdu->getPDULength(), port);
}

void coap_transmit::resetTimeout(){
    retransmission_count = 0;
    acktimer->start(ackTimeout);
}

void coap_transmit::timeout(){
    qDebug() << "Timeout";
    if(retransmission_count++ <= retransmissions){
        coap_engine* conn = coap_engine::getInstance();
        conn->send(addr, pdu->getPDUPointer(), pdu->getPDULength(), port);
        acktimer->start(ackTimeout*(1<<retransmission_count));
    }
    else{
        notResponding();
        emit tx_timeout();
    }
}

void coap_transmit::cleanupTimeout(){
    emit tx_timeout();

    qDebug() << "Clean up";
}

void coap_transmit::done(){
    acktimer->stop();
    cleanuptimer->stop();
}
