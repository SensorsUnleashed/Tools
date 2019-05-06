#ifndef COAP_TRANSACTION_H
#define COAP_TRANSACTION_H
#include <QObject>
#include <QTimer>
#include "cantcoap/cantcoap.h"
#include <node.h>
#include "coap_server.h"
#include "coap_blocktransfer.h"

class coap_transmit : public QObject
{
    Q_OBJECT

public:
    coap_transmit(QHostAddress addr, quint16 port);
    ~coap_transmit();

    int transmitHandler(CoapPDU *pdu);
    void transmitOnly(CoapPDU* pdu);
    void done();

    quint16 getPort(){ return port; }
    QHostAddress getAddr(){ return addr; }

protected:
    QHostAddress addr;
    quint16 port;

    void resetTimeout();
    virtual void notResponding() { }

private:

    int ackTimeout;
    int8_t retransmissions;
    int8_t retransmission_count;
    QTimer* acktimer;
    QTimer* cleanuptimer;
    CoapPDU *pdu;


private slots:
    void timeout();
    void cleanupTimeout();

signals:
    void tx_timeout();
};

class coap_transaction : public coap_transmit
{
    Q_OBJECT

public:
    coap_transaction(QHostAddress addr, quint16 port);
    int checkPDU(QHostAddress addr, CoapPDU *pdu);
    void sendACK(CoapPDU* recvPDU);

    virtual int update(CoapPDU *recvPDU){ Q_UNUSED(recvPDU); qDebug() << "coap_transaction::update Implement this!"; return 0;}

protected:
    suinterface* interface = nullptr;
    CoapPDU *pdu;
    enum CoapPDU::ContentFormat req_ct = CoapPDU::COAP_CONTENT_FORMAT_TEXT_PLAIN;

private slots:

};

class coap_client_transaction : public coap_transaction
{
    Q_OBJECT
public:
    coap_client_transaction(QHostAddress addr, quint16 port, CoapPDU *pdu, suinterface* interface = nullptr, QByteArray payload=nullptr);

    int update(CoapPDU *recvPDU);

protected:
        QByteArray uri;
    void notResponding();

    coap_blocktransfer* block1 = nullptr;
    coap_blockrx* block2 = nullptr;
};

class coap_server_transaction : public coap_transaction
{
    Q_OBJECT
public:
    coap_server_transaction(QHostAddress addr, quint16 port, CoapPDU *pdu, coap_server* interface = nullptr, QByteArray payload=nullptr);

    int update(CoapPDU *recvPDU);

protected:
    void notResponding();

    coap_blocktransfer* block2 = nullptr;

private:
    coap_server* serverif;
};


#endif // COAP_TRANSACTION_H
