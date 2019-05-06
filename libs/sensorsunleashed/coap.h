#ifndef COAP_H
#define COAP_H
#include "sensorsunleashed_global.h"
#include "cantcoap/cantcoap.h"

#define DEFAULT_COAP_PORT   5683

namespace coap {

    const uint32_t prefMsgSize = 32;

    CoapPDU::CoapOption* check_option(CoapPDU *pdu, enum CoapPDU::Option opt);
    int parseBlockOption(CoapPDU::CoapOption* blockoption, uint8_t* more, uint32_t* num, uint8_t* SZX);
    int parse_contentformat(CoapPDU* pdu, enum CoapPDU::ContentFormat* ct);
    int calc_block_option(uint8_t more, uint32_t num, uint32_t msgsize, uint8_t* blockval, uint16_t* len);
    int copyPDU(CoapPDU* src, CoapPDU* dst);

}

SENSORSUNLEASHEDSHARED_EXPORT CoapPDU::CoapOption* coap::check_option(CoapPDU *pdu, enum CoapPDU::Option opt);
SENSORSUNLEASHEDSHARED_EXPORT int coap::parseBlockOption(CoapPDU::CoapOption* blockoption, uint8_t* more, uint32_t* num, uint8_t* SZX);
SENSORSUNLEASHEDSHARED_EXPORT int coap::parse_contentformat(CoapPDU* pdu, enum CoapPDU::ContentFormat* ct);
SENSORSUNLEASHEDSHARED_EXPORT int coap::calc_block_option(uint8_t more, uint32_t num, uint32_t msgsize, uint8_t* blockval, uint16_t* len);
SENSORSUNLEASHEDSHARED_EXPORT int coap::copyPDU(CoapPDU* src, CoapPDU* dst);

#endif // COAP_H
