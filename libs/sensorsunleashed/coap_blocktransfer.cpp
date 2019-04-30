#include "coap_blocktransfer.h"

coap_blocktransfer::coap_blocktransfer(CoapPDU *pdu, enum CoapPDU::Option blocktype, QByteArray payload)
{
    this->blocktype = blocktype;
    tx_payload.append(payload);
    uint8_t buf[3];
    uint16_t len;
    coap::calc_block_option(1, 0, coap::prefMsgSize, &buf[0], &len);
    pdu->addOption(blocktype, len, &buf[0]);
    pdu->setPayload(reinterpret_cast<uint8_t*>(tx_payload.data()), static_cast<int>(coap::prefMsgSize));
    tx_next_index = coap::prefMsgSize;
    num = 0;
    more = 1;

    qDebug() << "Block: " << num << "/" << 1 << "/" << coap::prefMsgSize;
}

int coap_blocktransfer::next(CoapPDU* txPDU, CoapPDU::CoapOption* options){
    uint32_t num;
    uint8_t szx;
    uint32_t prefsize;
    uint32_t bytesleft = static_cast<uint32_t>(tx_payload.length()) - tx_next_index;

    //Sender send us some options to use from now on
    if(coap::parseBlockOption(options, &more, &num, &szx) == 0){
        prefsize = 1 << (szx + 4);
        qDebug() << "Block2: " << num << "/" << more << "/" << prefsize;
    }
    else{   //We continue with whatever options we started with
        num = this->num;
        prefsize = coap::prefMsgSize;
        qDebug() << "Block: " << num << "/" << more << "/" << prefsize << " else";
    }

    if(bytesleft){
        //We need to send yet another block
        uint8_t buf[3];
        uint16_t len;
        uint8_t* bufptr = &buf[0];

        more = bytesleft > prefsize;
        coap::calc_block_option(more, num, prefsize, bufptr, &len);

        txPDU->addOption(blocktype, len, bufptr);

        //storedPDUdata->txtime.restart();
        this->num = num;
        //tx_progress(storedPDUdata->tx_next_index, storedPDUdata->tx_payload.length());
        if(more){
            txPDU->setPayload(reinterpret_cast<uint8_t*>((tx_payload.data()+tx_next_index)), static_cast<int>(prefsize));
            tx_next_index += prefsize;

        }
        else{
            txPDU->setPayload(reinterpret_cast<uint8_t*>((tx_payload.data()+tx_next_index)), static_cast<int>(bytesleft));
            tx_next_index += bytesleft;
        }
    }

    return more;
}


coap_blockrx::coap_blockrx(){
}

int coap_blockrx::assemble(CoapPDU::CoapOption* options, uint8_t* rxdata, uint32_t rxlen){
    uint32_t num;
    uint8_t szx;
    uint8_t more;
    if(coap::parseBlockOption(options, &more, &num, &szx) == 0){
        uint32_t offset = num << (szx + 4);

        qDebug() << "Block2: " << num << "/" << more << "/" << (1 << (szx + 4));

        for(uint32_t i=0; i<rxlen; i++){
            rx_payload[offset + i] = static_cast<char>(*(rxdata+i));
        }
        if(more){
            //qDebug() << "Received " << num + 1 << "messages, so far";
            uint8_t* value = options->optionValuePointer;
            uint16_t valuelen = options->optionValueLength;
            ++num;


            *value &= 0x7;
            *value |= num << 4;
            //Clear all but the SXZ part
            for(int i=1; i<valuelen; i++){
                *(value+i)= 0;
                *value |= (num << (i * 8 + 4));
            }
        }
    }
    return more;
}

int coap_blockrx::next(CoapPDU* rxPDU, CoapPDU::CoapOption* options){
    rxPDU->addOption(CoapPDU::COAP_OPTION_BLOCK2, options->optionValueLength, options->optionValuePointer);
    return 0;
}

QByteArray coap_blockrx::getMessage(){
    return rx_payload;
}


