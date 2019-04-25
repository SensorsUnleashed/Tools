#include "coap.h"

CoapPDU::CoapOption* coap::check_option(CoapPDU *pdu, enum CoapPDU::Option opt){
    CoapPDU::CoapOption* options = pdu->getOptions();
    int len = pdu->getNumOptions();

    for(int i=0; i<len; i++){
        if((options+i)->optionNumber == opt){
            return options+i;
        }
    }
    return nullptr;
}

int coap::parseBlockOption(CoapPDU::CoapOption* blockoption, uint8_t* more, uint32_t* num, uint8_t* SZX){
    int len = blockoption->optionValueLength;
    uint8_t* value = blockoption->optionValuePointer;
    uint32_t result = 0;

    //To Host byte order
    for(int i=0; i<len; i++){
        result <<= 8;
        result |= *(value+i);
    }
    if(len >= 1){
        *more = (result & 0x8) > 0;
        *SZX = (result & 0x3);
        *num = result >> 4;
        *num |= result >> 4;
    }
    else{
        return 1;
    }
    return 0;
}

int coap::parse_contentformat(CoapPDU* pdu, enum CoapPDU::ContentFormat* ct){
    CoapPDU::CoapOption* options = coap::check_option(pdu, CoapPDU::COAP_OPTION_CONTENT_FORMAT);
    if(options != nullptr){
        if(options->optionValueLength > 0){
            *ct = static_cast<enum CoapPDU::ContentFormat>(*options->optionValuePointer);
            return 0;
        }
    }

    return 1;
}

#include  <QtEndian>
int coap::calc_block_option(uint8_t more, uint32_t num, uint32_t msgsize, uint8_t* blockval, uint16_t* len){
    /*
        We store in little, and let cantcoap send it big endian
        Illustration is in big endian. Network byte order
           0
           0 1 2 3 4 5 6 7
          +-+-+-+-+-+-+-+-+
          |  NUM  |M| SZX |
          +-+-+-+-+-+-+-+-+

           0                   1
           0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |          NUM          |M| SZX |
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

           0               1                   2
           0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |                   NUM                 |M| SZX |
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    SZX = exponetial 4-10 (16 - 1024 bytes)
    More = Will further blocks follow this
    NUM = Current block number (0 is the first)
*/

    uint32_t result = 0;

    //Calculate the exponential part
    uint16_t szx = static_cast<uint16_t>(msgsize >> 5);

    result |= num;
    result <<= 4;
    result |= szx + static_cast<uint8_t>((more << 3));

    if (num < 16){
        *len = 1;
        *(blockval+0) = static_cast<uint8_t>(result);
    }
    else if(num < 4096){
        *len = 2;
        *(blockval+0) = static_cast<uint8_t>(result >> 8);
        *(blockval+1) = static_cast<uint8_t>(result);
    }
    else{
        *len = 3;
        *(blockval+0) = static_cast<uint8_t>(result >> 16);
        *(blockval+1) = static_cast<uint8_t>(result >> 8);
        *(blockval+2) = static_cast<uint8_t>(result >> 0);
    }

    return 0;
}
