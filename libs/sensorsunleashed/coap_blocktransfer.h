#ifndef COAP_BLOCKTRANSFER_H
#define COAP_BLOCKTRANSFER_H

#include "coap.h"
#include <QtDebug>
#include <QByteArray>

class coap_blocktransfer
{
public:
    coap_blocktransfer(CoapPDU *pdu, enum CoapPDU::Option blocktype, QByteArray payload);
    int next(CoapPDU* txPDU, CoapPDU::CoapOption *options);
    uint8_t hasMore(){ return more; }
private:
    QByteArray tx_payload;
    uint32_t tx_next_index;
    uint32_t num;
    uint8_t more = 0;
    enum CoapPDU::Option blocktype;
};

class coap_blockrx
{
public:
    coap_blockrx();
    int assemble(CoapPDU::CoapOption* options, uint8_t* rxdata, uint32_t rxlen);
    int next(CoapPDU* rxPDU, CoapPDU::CoapOption* options);
    QByteArray getMessage();

private:
    QByteArray rx_payload;
    uint8_t blockoption[3];
    uint16_t blocklen;
};

#endif // COAP_BLOCKTRANSFER_H
