#ifndef COAP_H
#define COAP_H
#include "sensorsunleashed_global.h"
#include "cantcoap/cantcoap.h"

namespace coap {
    CoapPDU::CoapOption* check_option(CoapPDU *pdu, enum CoapPDU::Option opt);
}

//class coap {
//    CoapPDU::CoapOption* check_option(CoapPDU *pdu, enum CoapPDU::Option opt);
//};

SENSORSUNLEASHEDSHARED_EXPORT CoapPDU::CoapOption* coap::check_option(CoapPDU *pdu, enum CoapPDU::Option opt);
#endif // COAP_H
