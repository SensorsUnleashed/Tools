#include "coap_transaction.h"
#include <string.h>
#include "coap_engine.h"
#include "coap.h"

coap_server_transaction::coap_server_transaction(QHostAddress addr, quint16 port, CoapPDU *pdu, coap_server *interface, QByteArray payload) : coap_transaction(addr, port){
    this->pdu = pdu;
    this->serverif = interface;

    /*Add the payload if there is any */
    if(!payload.isEmpty()){
        if(payload.length() > static_cast<int>(coap::prefMsgSize)){  //Payload needs to be split
            block2 = new coap_blocktransfer(pdu, CoapPDU::COAP_OPTION_BLOCK2, *&payload);
        }
        else{   //Normal single message payload
            pdu->setPayload(reinterpret_cast<uint8_t*>(payload.data()), payload.length());
        }
    }

    //Store contentformat that we are requesting
    coap::parse_contentformat(pdu, &req_ct);

    if(transmitHandler(pdu) == 0){
        /* We dont expect anything in return. */
        delete this;
    }
    else{
        /* Ask the engine to keep an eye out for any data that belongs to us */
        coap_engine* conn = coap_engine::getInstance();
        conn->transactionListAdd(this);
    }
}

//Return 0 if we do not expect any more data regarding this pdu
//Return 1 otherwise
int coap_server_transaction::update(CoapPDU *recvPDU){
    int ret = 0;

    CoapPDU *txPDU = nullptr; //Assign this pdu to the next pdu to send, and switch out with the one in the store
    CoapPDU::CoapOption* options = nullptr;

    done();

    //nodeResponding(storedPDUdata->tokenref);
    CoapPDU::Code code = recvPDU->getCode();
    if(code >= CoapPDU::COAP_BAD_REQUEST)
        qDebug() << "Code: " << code;

    options = coap::check_option(recvPDU, CoapPDU::COAP_OPTION_BLOCK2);
    if(options != nullptr){
        if(block2){
            if(block2->hasMore()){
                txPDU = new CoapPDU();
                block2->next(txPDU, options);
            }
        }
        else{
            qDebug() << "ERR: There is a block2 option, but we never created one!!!";
        }
    }   //Block2 handling

    //Do the transmitting part
    if(txPDU){
        //Request the same content format, as it sends us
        options = coap::check_option(pdu, CoapPDU::COAP_OPTION_CONTENT_FORMAT);
        if(options){
            if(options->optionValueLength > 0){
                txPDU->setContentFormat(static_cast<enum CoapPDU::ContentFormat>(*options->optionValuePointer));
            }
        }

        txPDU->setMessageID(recvPDU->getMessageID() + 1);
        txPDU->setToken(pdu->getTokenPointer(), static_cast<uint8_t>(pdu->getTokenLength()));
        txPDU->setType(CoapPDU::COAP_ACKNOWLEDGEMENT);
        txPDU->setCode(pdu->getCode());

        //Switch out the old pdu with the new
        delete pdu;
        pdu = txPDU;
        ret = transmitHandler(pdu);
    }

    return ret;
}

void coap_server_transaction::notResponding(){
    serverif->receiverNotResponding(addr, port);
}


coap_client_transaction::coap_client_transaction(QHostAddress addr, quint16 port, CoapPDU *pdu, suinterface* interface, QByteArray payload) : coap_transaction(addr, port){
    this->interface = interface;
    this->pdu = pdu;

    uri.resize(200);
    int urilen;
    pdu->getURI(uri.data(), 200, &urilen);
    uri.resize(urilen);

    /*Add the payload if there is any */
    if(!payload.isEmpty()){
        if(payload.length() > static_cast<int>(coap::prefMsgSize)){  //Payload needs to be split
            block1 = new coap_blocktransfer(pdu, CoapPDU::COAP_OPTION_BLOCK1, *&payload);
        }
        else{   //Normal single message payload
            pdu->setPayload(reinterpret_cast<uint8_t*>(payload.data()), payload.length());
        }
    }
    //Store contentformat that we are requesting
    coap::parse_contentformat(pdu, &req_ct);

    if(transmitHandler(pdu) == 0){
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
    if(strncmp(reinterpret_cast<const char*>(pdu->getTokenPointer()), reinterpret_cast<const char*>(this->pdu->getTokenPointer()), static_cast<size_t>(this->pdu->getTokenLength())) != 0)
        return 3;
    return 0;
}

void coap_transaction::sendACK(CoapPDU* recvPDU){
    CoapPDU *ackPDU = new CoapPDU();

    //ackPDU->setURI(uri.data(), static_cast<size_t>(uri.length()));
    ackPDU->setMessageID(recvPDU->getMessageID());
    ackPDU->setToken(recvPDU->getTokenPointer(), static_cast<uint8_t>(recvPDU->getTokenLength()));
    ackPDU->setType(CoapPDU::COAP_ACKNOWLEDGEMENT);
    transmitOnly(ackPDU);
    delete ackPDU;
}

/* Return 0 if it is done - 1 if it needs more messages */
int coap_client_transaction::update(CoapPDU *recvPDU){
    int ret = 0;
    CoapPDU *txPDU = nullptr; //Assign this pdu to the next pdu to send, and switch out with the one in the store
    CoapPDU::CoapOption* options = nullptr;
    CoapPDU::Code code = recvPDU->getCode();

    done();

    if(code == CoapPDU::COAP_EMPTY){
        //This means the server knows about us, but is not ready to provide us with what we want. Wait for it
        resetTimeout();
        return 1;
    }

    options = coap::check_option(recvPDU, CoapPDU::COAP_OPTION_OBSERVE);
    if(options != nullptr){
        ret = 1;
    }

    //Send large block to a server
    options = coap::check_option(recvPDU, CoapPDU::COAP_OPTION_BLOCK1);
    if(options != nullptr){
        if(block1){
            if(block1->hasMore()){
                if(!txPDU) txPDU = new CoapPDU();
                block1->next(txPDU, options);
            }
        }
        else{
            qDebug() << "ERR: There is a block2 option, but we never created one!!!";
        }
    }   //Block2 handling

    //Receive large block from a server
    options = coap::check_option(recvPDU, CoapPDU::COAP_OPTION_BLOCK2);
    if(options != nullptr){
        if(!block2) block2 = new coap_blockrx();
        if(block2->assemble(options, recvPDU->getPayloadPointer(), static_cast<uint32_t>(recvPDU->getPayloadLength())) > 0){
            if(!txPDU) txPDU = new CoapPDU();
            block2->next(txPDU, options);
        }
        else{ //Finish receiving
            enum CoapPDU::ContentFormat ct;
            if(coap::parse_contentformat(recvPDU, &ct) == 1){
                ct = req_ct;
            }
            interface->parseMessage(QByteArray::fromRawData(reinterpret_cast<const char*>(recvPDU->getTokenPointer()), recvPDU->getTokenLength()), block2->getMessage(), code, ct);
        }
    }
    else{ //Its just a regular piggibacked ack
        enum CoapPDU::ContentFormat ct = CoapPDU::COAP_CONTENT_FORMAT_TEXT_PLAIN;
        //Handle single messages
        if(coap::parse_contentformat(recvPDU, &ct) == 1){
            ct = req_ct;
        }

        if(recvPDU->getPayloadLength()){
            interface->parseMessage(QByteArray::fromRawData(reinterpret_cast<const char*>(recvPDU->getTokenPointer()), recvPDU->getTokenLength()), QByteArray::fromRawData(reinterpret_cast<char*>(recvPDU->getPayloadPointer()), recvPDU->getPayloadLength()), code, ct);
        }
        else{
            interface->parseMessage(QByteArray::fromRawData(reinterpret_cast<const char*>(recvPDU->getTokenPointer()), recvPDU->getTokenLength()), QByteArray(), code, ct);
        }
    }

    //Do the transmitting part
    if(txPDU){
        //Request the same content format, as it sends us
        options = coap::check_option(pdu, CoapPDU::COAP_OPTION_CONTENT_FORMAT);
        if(options){
            if(options->optionValueLength > 0){
                txPDU->setContentFormat(static_cast<enum CoapPDU::ContentFormat>(*options->optionValuePointer));
            }
        }

        txPDU->setMessageID(recvPDU->getMessageID() + 1);
        txPDU->setToken(pdu->getTokenPointer(), static_cast<uint8_t>(pdu->getTokenLength()));
        txPDU->setType(pdu->getType());
        txPDU->setCode(pdu->getCode());
        txPDU->setURI(uri.data(), static_cast<size_t>(uri.length()));

        //Switch out the old pdu with the new
        delete pdu;
        pdu = txPDU;
        ret |= transmitHandler(pdu);
    }
    else if(recvPDU->getType() == CoapPDU::COAP_CONFIRMABLE){
        sendACK(recvPDU);
    }
    return ret;
}

void coap_client_transaction::notResponding(){
    interface->noResponse();
}

