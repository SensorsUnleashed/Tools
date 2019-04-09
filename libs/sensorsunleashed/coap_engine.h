#ifndef COAP_ENGINE_H
#define COAP_ENGINE_H

#include "wsn.h"
#include "coap_transaction.h"


class coap_engine : public socket
{
    Q_OBJECT

public:
    coap_engine();

    static coap_engine* getInstance();

    void addServerInstance(coap_server* ref);
    void rmServerInstance(coap_server* ref);

    void transactionListAdd(coap_transaction* transaction);

    void receive(QHostAddress addr, QByteArray datagram, quint16 port);

private:
    coap_server* serverref = nullptr;

    static bool instanceFlag;
    static coap_engine *coapEngine;

    QVector<coap_transaction*> transactionList;


    //QUdpSocket* udpSocket;
    //QVector<struct observer> observerlist;

public slots:
    void transactionTimeout();

};

#endif // COAP_ENGINE_H
