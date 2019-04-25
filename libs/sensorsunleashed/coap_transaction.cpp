#include "coap_transaction.h"
#include <string.h>
#include "coap_engine.h"
#include "coap.h"

coap_server_transaction::coap_server_transaction(QHostAddress addr, quint16 port, CoapPDU *pdu, coap_server *interface, QByteArray payload) : coap_transaction(addr, port){
    this->pdu = pdu;
    this->tx_payload = payload;
    this->serverif = interface;

    prefMsgSize = 32;   //Should be taken from the db

    qDebug() << "Server: Ready to send to " << addr.toString();

    /*Add the payload if there is any */
    if(!payload.isEmpty()){
        if(payload.length() > static_cast<int>(prefMsgSize)){  //Payload needs to be split
            uint8_t buf[3];
            uint16_t len;
            coap::calc_block_option(1, 0, prefMsgSize, &buf[0], &len);
            pdu->addOption(CoapPDU::COAP_OPTION_BLOCK2, len, &buf[0]);
            pdu->setPayload(reinterpret_cast<uint8_t*>(payload.data()), static_cast<int>(prefMsgSize));
            tx_next_index = prefMsgSize;
            num = 0;

            qDebug() << "Block2: " << num << "/" << 1 << "/" << prefMsgSize;

            //tx_progress(storedPDU->num, payload.length());
        }
        else{   //Normal single message payload
            pdu->setPayload(reinterpret_cast<uint8_t*>(payload.data()), payload.length());
        }
    }
    //Store contentformat that we are requesting
    coap::parse_contentformat(pdu, &req_ct);

    if(transmit(pdu) == 0){
        /* We dont expect anything in return. */
        delete this;
    }
    else{
        /* Ask the engine to keep an eye out for any data that belongs to us */
        coap_engine* conn = coap_engine::getInstance();
        conn->transactionListAdd(this);
    }
    /* Reset the progressbar */
    //emit timeoutinfo(0, retransmissions);
}

int coap_server_transaction::update(CoapPDU *recvPDU){
    int ret;
    uint8_t more = 0;

    CoapPDU *txPDU; //Assign this pdu to the next pdu to send, and switch out with the one in the store
    CoapPDU::CoapOption* options = nullptr;
    int dotx = 0;

    done();

    //nodeResponding(storedPDUdata->tokenref);
    CoapPDU::Code code = recvPDU->getCode();
    if(code >= CoapPDU::COAP_BAD_REQUEST)
        qDebug() << "Code: " << code;

    //Handle block2 - response to a get
    options = coap::check_option(recvPDU, CoapPDU::COAP_OPTION_BLOCK2);
    if(options != nullptr){
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
            prefsize = prefMsgSize;
            qDebug() << "Block2: " << num << "/" << more << "/" << prefsize << " else";
        }

        if(bytesleft){
            //We need to send yet another block
            txPDU = new CoapPDU();

            uint8_t buf[3];
            uint16_t len;
            uint8_t* bufptr = &buf[0];

            more = bytesleft > prefsize;
            coap::calc_block_option(more, num, prefsize, bufptr, &len);

            txPDU->addOption(CoapPDU::COAP_OPTION_BLOCK2, len, bufptr);

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

            dotx = 1;
        }
        else{
            qDebug() << "ACK - Finished transmitting large message";
            //tx_progress(storedPDUdata->tx_payload.length(), storedPDUdata->tx_payload.length());
            //enableTokenRemoval(storedPDUdata->token);
        }
    }   //Block2 handling

    //Do the transmitting part
    if(dotx){
        //Request the same content format, as it sends us
        options = coap::check_option(pdu, CoapPDU::COAP_OPTION_CONTENT_FORMAT);
        if(options){
            if(options->optionValueLength > 0){
                txPDU->setContentFormat((enum CoapPDU::ContentFormat)*options->optionValuePointer);
            }
        }

        txPDU->setMessageID(recvPDU->getMessageID() + 1);
        txPDU->setToken(pdu->getTokenPointer(), pdu->getTokenLength());
        txPDU->setType(pdu->getType());
        txPDU->setCode(pdu->getCode());

        QByteArray uri(200, 0);
        int urilen;
        pdu->getURI(uri.data(), 200, &urilen);
        txPDU->setURI(uri.data(), urilen);

        //Switch out the old pdu with the new
        delete pdu;
        pdu = txPDU;
        ret = transmit(pdu) || more;
    }

    return ret;
}

void coap_server_transaction::notResponding(){
    serverif->receiverNotResponding(addr, port);
}


coap_client_transaction::coap_client_transaction(QHostAddress addr, quint16 port, CoapPDU *pdu, suinterface* interface, QByteArray payload) : coap_transaction(addr, port){
    this->interface = interface;
    this->pdu = pdu;
    this->tx_payload = payload;

    prefMsgSize = 32;   //Should be taken from the db

    qDebug() << "Ready to send to " << addr.toString();

    /*Add the payload if there is any */
    if(!payload.isEmpty()){
        if(payload.length() > static_cast<int>(prefMsgSize)){  //Payload needs to be split
            uint8_t buf[3];
            uint16_t len;
            coap::calc_block_option(1, 0, prefMsgSize, &buf[0], &len);
            pdu->addOption(CoapPDU::COAP_OPTION_BLOCK1, len, &buf[0]);
            pdu->setPayload(reinterpret_cast<uint8_t*>(payload.data()), static_cast<int>(prefMsgSize));
            tx_next_index = prefMsgSize;
            num = 0;
            //tx_progress(storedPDU->num, payload.length());
        }
        else{   //Normal single message payload
            pdu->setPayload(reinterpret_cast<uint8_t*>(payload.data()), payload.length());
        }
    }
    //Store contentformat that we are requesting
    coap::parse_contentformat(pdu, &req_ct);

    if(transmit(pdu) == 0){
        /* We dont expect anything in return. */
        delete this;
    }
    else{
        /* Ask the engine to keep an eye out for any data that belongs to us */
        coap_engine* conn = coap_engine::getInstance();
        conn->transactionListAdd(this);
    }
    /* Reset the progressbar */
    //emit timeoutinfo(0, retransmissions);
}

coap_transaction::coap_transaction(QHostAddress addr, quint16 port) : coap_transmit(addr, port)
{
    this->addr = addr;
}

int coap_transaction::checkPDU(QHostAddress addr, CoapPDU *pdu){
    if(addr != this->addr)
        return 1;
    if(pdu->getTokenLength() != this->pdu->getTokenLength()){
        return 2;
    }
    if(strncmp((const char*)pdu->getTokenPointer(), (const char*) this->pdu->getTokenPointer(), this->pdu->getTokenLength() != 0))
        return 3;
    return 0;
}

void coap_transaction::send_ACK(CoapPDU *recvPDU){
    CoapPDU *ackPDU = new CoapPDU();

    QByteArray uri(200, 0);
    int urilen;
    ackPDU->getURI(uri.data(), 200, &urilen);
    ackPDU->setURI(uri.data(), urilen);
    ackPDU->setMessageID(recvPDU->getMessageID());
    ackPDU->setToken(recvPDU->getTokenPointer(), recvPDU->getTokenLength());
    ackPDU->setType(CoapPDU::COAP_ACKNOWLEDGEMENT);
    transmit(ackPDU);
    delete ackPDU;
}

/* Return 0 if it is done - 1 if it needs more messages */
int coap_client_transaction::update(CoapPDU *recvPDU){

    int ret = 0;
    CoapPDU *txPDU; //Assign this pdu to the next pdu to send, and switch out with the one in the store
    CoapPDU::CoapOption* options = nullptr;
    int dotx = 0;
    int wait = 0;
    int handled = 0;

    done();

            CoapPDU::Type type = recvPDU->getType();
    if(type == CoapPDU::COAP_CONFIRMABLE){
        send_ACK(recvPDU);
    }

    //nodeResponding(storedPDUdata->tokenref);
    CoapPDU::Code code = recvPDU->getCode();
    if(code >= CoapPDU::COAP_BAD_REQUEST)
        qDebug() << "Code: " << code;


    /* Handle block1 - response from a put/post (Send more money) */
    options = coap::check_option(recvPDU, CoapPDU::COAP_OPTION_BLOCK1);
    if(options != nullptr){
        handled = 1;
        uint8_t more;
        uint32_t num;
        uint8_t szx;
        uint32_t prefsize;
        uint32_t bytesleft = tx_payload.length() - tx_next_index;

        //Sender send us some options to use from now on
        if(coap::parseBlockOption(options, &more, &num, &szx) == 0){
            prefsize = 1 << (szx + 4);
            qDebug() << "Block1: " << num << "/" << more << "/" << prefsize;
        }
        else{   //We continue with whatever options we started with
            num = this->num;
            prefsize = prefMsgSize;
            qDebug() << "Block1: " << num << "/" << more << "/" << prefsize << " else";
        }

        if(bytesleft){
            //We need to send yet another block
            txPDU = new CoapPDU();

            uint8_t buf[3];
            uint16_t len;
            uint8_t* bufptr = &buf[0];

            more = bytesleft > prefsize;
            coap::calc_block_option(more, ++num, prefsize, bufptr, &len);

            txPDU->addOption(CoapPDU::COAP_OPTION_BLOCK1, len, bufptr);

            //storedPDUdata->txtime.restart();
            this->num = num;
            //tx_progress(storedPDUdata->tx_next_index, storedPDUdata->tx_payload.length());
            if(more){
                txPDU->setPayload((uint8_t*)(tx_payload.data()+tx_next_index), prefsize);
                tx_next_index += prefsize;

            }
            else{
                txPDU->setPayload((uint8_t*)(tx_payload.data()+tx_next_index), bytesleft);
                tx_next_index += bytesleft;
            }

            dotx = 1;
        }
        else{
            handled = 0;
            qDebug() << "ACK - Finished transmitting large message";
            //tx_progress(storedPDUdata->tx_payload.length(), storedPDUdata->tx_payload.length());
            //enableTokenRemoval(storedPDUdata->token);
        }
    }

    //Handle block2 - response to a get
    options = coap::check_option(recvPDU, CoapPDU::COAP_OPTION_BLOCK2);
    if(options != nullptr){
        handled = 1;
        uint8_t more;
        uint32_t num;
        uint8_t szx;
        if(coap::parseBlockOption(options, &more, &num, &szx) == 0){
            uint32_t offset = num << (szx + 4);

            qDebug() << "Block2: " << num << "/" << more << "/" << (1 << (szx + 4));

            uint8_t* pl = recvPDU->getPayloadPointer();
            for(int i=0; i<recvPDU->getPayloadLength(); i++){
                rx_payload[offset + i] = *(pl+i);
            }
            if(more){
                //qDebug() << "Received " << num + 1 << "messages, so far";
                uint8_t* value = options->optionValuePointer;
                uint8_t valuelen = options->optionValueLength;
                ++num;

                *value &= 0x7;
                *value |= num << 4;
                //Clear all but the SXZ part
                for(int i=1; i<valuelen; i++){
                    *(value+i)= 0;
                    *value |= (num << (i * 8 + 4));
                }

                txPDU = new CoapPDU();
                txPDU->addOption(CoapPDU::COAP_OPTION_BLOCK2, valuelen, value);
                dotx = 1;
            }
            else{
                enum CoapPDU::ContentFormat ct;
                if(coap::parse_contentformat(recvPDU, &ct) == 1){
                    ct = req_ct;
                }
                interface->parseMessage(QByteArray::fromRawData(reinterpret_cast<const char*>(recvPDU->getTokenPointer()), recvPDU->getTokenLength()), rx_payload, code, ct);
            }
        }
    }   //Block2 handling

    //Its just a regular piggibacked ack
    if(!handled){
        if(code == CoapPDU::COAP_EMPTY){
            //This means the server knows about us, but is not ready to provide us with what we want. Wait for it
            wait = 1;
        }
        else if(recvPDU->getPayloadLength()){
            uint8_t* pl = recvPDU->getPayloadPointer();
            for(int i=0; i<recvPDU->getPayloadLength(); i++){
                rx_payload[i] = *(pl+i);
            }
            //Handle single messages
            enum CoapPDU::ContentFormat ct;
            if(coap::parse_contentformat(recvPDU, &ct) == 1){
                ct = req_ct;
            }
            interface->parseMessage(QByteArray::fromRawData(reinterpret_cast<const char*>(recvPDU->getTokenPointer()), recvPDU->getTokenLength()), rx_payload, code, ct);
        }
        else{
            //handleReturnCode(storedPDUdata->tokenref, code);
        }
    }

    //Do the transmitting part
    if(dotx){
        //Request the same content format, as it sends us
        options = coap::check_option(pdu, CoapPDU::COAP_OPTION_CONTENT_FORMAT);
        if(options){
            if(options->optionValueLength > 0){
                txPDU->setContentFormat((enum CoapPDU::ContentFormat)*options->optionValuePointer);
            }
        }

        txPDU->setMessageID(recvPDU->getMessageID() + 1);
        txPDU->setToken(pdu->getTokenPointer(), pdu->getTokenLength());
        txPDU->setType(pdu->getType());
        txPDU->setCode(pdu->getCode());

        QByteArray uri(200, 0);
        int urilen;
        pdu->getURI(uri.data(), 200, &urilen);
        txPDU->setURI(uri.data(), urilen);

        //Switch out the old pdu with the new
        delete pdu;
        pdu = txPDU;
        transmit(pdu);
        ret = 1;
    }
    else if(wait){
        //Reset timeout
        //storedPDUdata->retranscount = 0;
        //storedPDUdata->txtime.restart();
    }
    else{
        ret = 0;

        options = coap::check_option(recvPDU, CoapPDU::COAP_OPTION_OBSERVE);
        if(options != nullptr){
            ret = 1;
        }
    }

    return ret;
}
