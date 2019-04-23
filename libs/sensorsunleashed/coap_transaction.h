#ifndef COAP_TRANSACTION_H
#define COAP_TRANSACTION_H
#include <QObject>

#include "cantcoap/cantcoap.h"
#include <node.h>
#include "coap_server.h"
#include "su_message.h"

class coap_transmit : public QObject
{
    Q_OBJECT

public:
    coap_transmit(QHostAddress addr, quint16 port);
    ~coap_transmit();

    int transmit(CoapPDU *pdu);
    void done();

private:
    QHostAddress addr;
    quint16 port;
    int ackTimeout;
    int8_t retransmissions;
    QTimer* acktimer;
    CoapPDU *pdu;


private slots:
    void timeout();

signals:
    void tx_timeout();
};

class coap_transaction : public coap_transmit
{
    Q_OBJECT

public:
    coap_transaction(QHostAddress addr, quint16 port);
    int checkPDU(QHostAddress addr, CoapPDU *pdu);

    virtual int update(CoapPDU *recvPDU){ Q_UNUSED(recvPDU); qDebug() << "coap_transaction::update Implement this!"; return 0;}

protected:
    suinterface* interface = nullptr;
    CoapPDU *pdu;
    QByteArray tx_payload;
    QByteArray rx_payload;
    uint32_t prefMsgSize;
    uint32_t tx_next_index; //What should be send next
    uint32_t num;
    enum CoapPDU::ContentFormat req_ct = CoapPDU::COAP_CONTENT_FORMAT_TEXT_PLAIN;

    int calc_block_option(uint8_t more, uint32_t num, uint32_t msgsize, uint8_t* blockval, uint16_t* len);
    int parse_contentformat(CoapPDU* pdu, enum CoapPDU::ContentFormat* ct);
    int parseBlockOption(CoapPDU::CoapOption* blockoption, uint8_t* more, uint32_t* num, uint8_t* SZX);
    void send_ACK(CoapPDU *recvPDU);
private:
    QHostAddress addr;

private slots:

};

class coap_client_transaction : public coap_transaction
{
    Q_OBJECT
public:
    coap_client_transaction(QHostAddress addr, quint16 port, CoapPDU *pdu, suinterface* interface = nullptr, QByteArray payload=nullptr);

    int update(CoapPDU *recvPDU);
};

class coap_server_transaction : public coap_transaction
{
    Q_OBJECT
public:
    coap_server_transaction(QHostAddress addr, quint16 port, CoapPDU *pdu, coap_server* interface = nullptr, QByteArray payload=nullptr);

    int update(CoapPDU *recvPDU);

private:
    coap_server* interface;
};


#endif // COAP_TRANSACTION_H
