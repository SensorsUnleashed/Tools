/*******************************************************************************
 * Copyright (c) 2017, Ole Nissen.
 *  All rights reserved. 
 *  
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met: 
 *  1. Redistributions of source code must retain the above copyright 
 *  notice, this list of conditions and the following disclaimer. 
 *  2. Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following
 *  disclaimer in the documentation and/or other materials provided
 *  with the distribution. 
 *  3. The name of the author may not be used to endorse or promote
 *  products derived from this software without specific prior
 *  written permission.  
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 *  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 *  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 *
 * This file is part of the Sensors Unleashed project
 *******************************************************************************/
#include "node.h"
//#include "helper.h"

int encode(char* buffer, cmp_object_t objTemplate, QVariant value);
static uint32_t buf_writer(cmp_ctx_t* ctx, const void *data, uint32_t count);

sensor::sensor(node* parent, QString uri, QVariantMap attributes) : suinterface(parent->getAddress())
{
    qDebug() << "Sensor: " << uri << " with attribute: " << attributes << " created";
    this->parent = parent;
    this->uri = uri;
    ip = parent->getAddress();
    eventsActive.as.u8 = 0;
    eventsActive.type = CMP_TYPE_UINT8;
    LastValue.as.s8 = 0;
    LastValue.type = CMP_TYPE_SINT8;
    AboveEventAt.as.s8 = 0;
    AboveEventAt.type = CMP_TYPE_SINT8;
    BelowEventAt.as.s8 = 0;
    BelowEventAt.type = CMP_TYPE_SINT8;
    ChangeEvent.as.s8 = 0;
    ChangeEvent.type = CMP_TYPE_SINT8;
    RangeMin.as.s8 = 0;
    RangeMin.type = CMP_TYPE_SINT8;
    RangeMax.as.s8 = 0;
    RangeMax.type = CMP_TYPE_SINT8;

    init = 0;
}

/* We create a dummy sensor, used only if a pairing could not be resolved */
sensor::sensor(QString ipaddr, QString uri): suinterface(QHostAddress(ipaddr)){
    ip = QHostAddress(ipaddr);
    this->uri = uri;
    parent = nullptr;
    init = 1;
}

void sensor::initSensor(){
    if(!init){
        requestRangeMin();
        requestRangeMax();
        req_eventSetup();
        init = 1;
    }
}

QVariant sensor::getConfigValues(){
    QVariantList list;

    QVariantMap result;
    result = cmpobjectToVariant(LastValue).toMap();
    result["id"] = "LastValue";
    list.append(result);
    result = cmpobjectToVariant(AboveEventAt).toMap();
    result["id"] = "AboveEventAt";
    list.append(result);
    result = cmpobjectToVariant(BelowEventAt).toMap();
    result["id"] = "BelowEventAt";
    list.append(result);
    result = cmpobjectToVariant(ChangeEvent).toMap();
    result["id"] = "ChangeEvent";
    list.append(result);
    result = cmpobjectToVariant(RangeMin).toMap();
    result["id"] = "RangeMin";
    list.append(result);
    result = cmpobjectToVariant(RangeMax).toMap();
    result["id"] = "RangeMax";
    list.append(result);
    result = cmpobjectToVariant(eventsActive).toMap();
    result["id"] = "eventsActive";
    list.append(result);

    return list;
}

void sensor::requestValue(char *query){
    char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(uristring, strlen(uristring));
    if(query){
        pdu->addURIQuery(query);
    }
    get_request(pdu, req_currentValue);
}

QVariant sensor::requestObserve(QString event){
    uint8_t id = 0;

    QString str = uri + "/" + event;
    QByteArray arr = QByteArray(str.toLatin1().data(), str.length());

    char* uristring = arr.data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(uristring, strlen(uristring));
    pdu->addOption(CoapPDU::COAP_OPTION_OBSERVE, 1, &id);

    char dst[100];
    int len;
    pdu->getURI(&dst[0], 100, &len);

    return get_request(pdu, req_observe);
}

void sensor::abortObserve(QVariant token){
    /*
        Removing a token, will render the next
        pdu as unknown and retransmit a RST command
    */
    enableTokenRemoval(token.toUInt());
}

void sensor::requestAboveEventLvl(){   
    char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(uristring, strlen(uristring));
    pdu->addURIQuery("AboveEventAt");
    get_request(pdu, req_aboveEventValue);
}

void sensor::requestBelowEventLvl(){   
    char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(uristring, strlen(uristring));
    pdu->addURIQuery("BelowEventAt");
    get_request(pdu, req_belowEventValue);
}

void sensor::requestChangeEventLvl(){   
    char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(uristring, strlen(uristring));
    pdu->addURIQuery("ChangeEventAt");
    get_request(pdu, req_changeEventAt);
}

void sensor::requestRangeMin(){
    char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(uristring, strlen(uristring));
    pdu->addURIQuery("RangeMin");
    get_request(pdu, req_RangeMinValue);
}

void sensor::requestRangeMax(){
    char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(uristring, strlen(uristring));
    pdu->addURIQuery("RangeMax");
    get_request(pdu, req_RangeMaxValue);
}

void sensor::req_eventSetup(){
    char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(uristring, strlen(uristring));
    pdu->addURIQuery("getEventSetup");
    get_request(pdu, req_getEventSetup);
}

void sensor::updateConfig(QVariant updatevalues){
    qDebug() << updatevalues.toMap();
    QVariantMap values = updatevalues.toMap();
    QByteArray payload(200, 0);
    int len = 0;

    //We store the values in the same containers as with what received
    if(!values.contains("AboveEventAt")) return;
    len += encode(payload.data() + len, AboveEventAt, values["AboveEventAt"]);
    if(!values.contains("BelowEventAt")) return;
    len += encode(payload.data() + len, BelowEventAt, values["BelowEventAt"]);
    if(!values.contains("ChangeEvent")) return;
    len += encode(payload.data() + len, ChangeEvent, values["ChangeEvent"]);
    if(!values.contains("eventsActive")) return;
    len += encode(payload.data() + len, eventsActive, values["eventsActive"]);

    payload.resize(len);

    char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(uristring, strlen(uristring));
    pdu->addURIQuery("eventsetup");

    put_request(pdu, req_updateEventsetup, payload);
}

void sensor::flashSave(){
    char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(uristring, strlen(uristring));
    pdu->addURIQuery("saveSetup");
    get_request(pdu, req_saveSetup);
}

void sensor::getpairingslist(){
    char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(uristring, strlen(uristring));
    pdu->addURIQuery("pairings");
    get_request(pdu, req_pairingslist);
}

QVariant sensor::clearpairingslist(){
    char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(uristring, strlen(uristring));
    pdu->addURIQuery("pairRemoveAll");
    return put_request(pdu, req_clearparings, nullptr);
}

uint16_t sensor::removeItems(QByteArray arr){
    char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(uristring, strlen(uristring));
    pdu->addURIQuery("pairRemoveIndex");

    QByteArray payload;
    payload.resize(200);

    cmp_ctx_t cmp;
    cmp_init(&cmp, payload.data(), buf_reader, buf_writer);

    cmp_write_array(&cmp, arr.size());
    for(uint8_t i=0; i<arr.size(); i++){
        cmp_write_u8(&cmp, arr[i]);
    }
    payload.resize(static_cast<int>(static_cast<uint8_t*>(cmp.buf) - reinterpret_cast<uint8_t*>(payload.data())));
    return put_request(pdu, req_removepairingitems, payload);
}

QVariant sensor::pair(QVariant pairdata){

    QVariantMap map = pairdata.toMap();

    //Find out if all neccessary informations is available
    if(!map.contains("triggers")) return QVariant(-1);
    if(!map.contains("addr")) return QVariant(-1);
    if(!map.contains("dsturi")) return QVariant(-1);

    QHostAddress pairaddr(map["addr"].toString());
    if(pairaddr.isNull()) return QVariant(-1);
    Q_IPV6ADDR addr = pairaddr.toIPv6Address();

    QByteArray triggersetup;
    triggersetup[0] = -1;
    triggersetup[1] = -1;
    triggersetup[2] = -1;

    QVariantList triggers = map["triggers"].toList();
    foreach (QVariant trigger, triggers) {
        if(trigger.toMap()["eventname"].toString().compare("Above event") == 0)
            triggersetup[0] = static_cast<int8_t>(trigger.toMap()["actionenum"].toInt());
        if(trigger.toMap()["eventname"].toString().compare("Below event") == 0)
            triggersetup[1] = static_cast<int8_t>(trigger.toMap()["actionenum"].toInt());
        if(trigger.toMap()["eventname"].toString().compare("Change event") == 0)
            triggersetup[2] = static_cast<int8_t>(trigger.toMap()["actionenum"].toInt());
    }

    QByteArray pairurlstr = map["dsturi"].toString().toLatin1();
    if(!pairurlstr.length()) return QVariant(-1);

    QByteArray payload;
    payload.resize(200);

    cmp_ctx_t cmp;
    cmp_init(&cmp, payload.data(), buf_reader, buf_writer);

    /*
     * If the prefix is the same for the pair address and
     * the the node, no need to send the prefix.
     * If not we just send it all.
    */
    int pl = parent->getPrefixlen();
    if(parent->getAddress().isEqual(pairaddr)){ //If its the localhost, send ::1
        QByteArray pairaddrarr;
        pairaddrarr.append(static_cast<char>(0));
        pairaddrarr.append(static_cast<char>(1));

        cmp_write_array(&cmp, pairaddrarr.length());
        for(int i=0; i<pairaddrarr.length(); i +=2){
            cmp_write_u16(&cmp, ((uint8_t)(pairaddrarr[i+1]) << 8) + (uint8_t)pairaddrarr[i]);
        }
    }
    else if(parent->getAddress().isInSubnet(pairaddr, pl)){ //Send all but the prefix
        //2000::/10
        //Start = 2000:0000:0000:0000:0000:0000:0000
        //End =   203f:ffff:ffff:ffff:ffff:ffff:ffff:ffff

        //MASK  11111111.11000000.000000....    = /10
        //START 00100000.00000000....           = 2000.0000.....
        //END   00100000.00111111.11111111...   = 203f.ffff.ffff.....

        //Only send the suffix
        QByteArray pairaddrarr;
        for(int i=0; i<16; i++){
            if(pl == 0){
                pairaddrarr.append(addr[i]);
            }
            else if(pl < 8){
                pairaddrarr.append(addr[i] & (0xFF << pl));
                pl = 0;
            }
            else{
                pl -= 8;
            }
        }
        //Add a 0 byte to the front of the array, so that we
        //can transmit it as a 16bit array. It lowers the total
        //byte count
        if(pairaddrarr.length() % 2 != 0) pairaddrarr.prepend((char)0);
        cmp_write_array(&cmp, pairaddrarr.length());
        for(int i=0; i<pairaddrarr.length(); i +=2){
            cmp_write_u16(&cmp, ((uint8_t)(pairaddrarr[i+1]) << 8) + (uint8_t)pairaddrarr[i]);
        }
    }
    else{
        cmp_write_array(&cmp, 16);
        for(int i=0; i<16; i += 2){
            cmp_write_u16(&cmp, (addr[i+1] << 8) + addr[i]);
        }
    }

    cmp_write_str(&cmp, pairurlstr.data(), pairurlstr.length());

    //Add the event triggers
    cmp_write_array(&cmp, 3);
    cmp_write_s8(&cmp, triggersetup[0]);    //Above action pointer
    cmp_write_s8(&cmp, triggersetup[1]);    //Below action pointer
    cmp_write_s8(&cmp, triggersetup[2]);    //Change action pointer

    payload.resize(static_cast<int>(static_cast<uint8_t*>(cmp.buf) - reinterpret_cast<uint8_t*>(payload.data())));

    char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(uristring, strlen(uristring));
    pdu->addURIQuery("join");

    return put_request(pdu, req_pairsensor, payload);
}

void sensor::testEvents(QVariant event, QVariant value){
    QString e = event.toString();
    QVariantMap m = value.toMap();
    QByteArray payload;
    payload.reserve(100);
    uint8_t eventval = 8;
    if(e.compare("aboveEvent") == 0){
        eventval = 2;
    }
    else if(e.compare("belowEvent") == 0){
        eventval = 4;
    }
    else if(e.compare("changeEvent") == 0){
        eventval = 8;
    }

    cmp_ctx_t cmp;
    cmp_init(&cmp, payload.data(), nullptr, buf_writer);

    cmp_write_u8(&cmp, eventval);
    cmp_write_u8(&cmp, 0);  //For now just send a zero as payload
    payload.resize(static_cast<int>(static_cast<uint8_t*>(cmp.buf) - reinterpret_cast<uint8_t*>(payload.data())));

    char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(uristring, strlen(uristring));
    pdu->addURIQuery("postEvent");

    put_request(pdu, req_testevent, payload);
}

/******** Sensor reply handlers ************/
void sensor::handleReturnCode(msgid token, CoapPDU::Code code){

    if(token.req == req_clearparings){
        if(code == CoapPDU::COAP_CHANGED){
            //pairings->clear();
        }
    }
    else if(token.req == req_removepairingitems){
        qDebug() << "handleReturnCode req_removepairingitems";
        //pairings->removePairingsAck(token.number);
    }
    else if(token.req == req_observe){
        qDebug() << "handleReturnCode req_observe: " << code;
    }
    else if(token.req == req_pairsensor){
        qDebug() << "handleReturnCode req_pairsensor: " << code;
    }
}

void sensor::nodeNotResponding(msgid token){
    qDebug() << "Token " << token.number << " timed out";
}

QVariant sensor::parseAppOctetFormat(msgid token, QByteArray payload, CoapPDU::Code code) {
    int cont = 0;
    cmp_ctx_t cmp;
    cmp_init(&cmp, payload.data(), buf_reader, nullptr);

    do{
        cmp_object_t obj;
        if(!cmp_read_object(&cmp, &obj)) return QVariant(0);
        QVariantMap result = cmpobjectToVariant(obj, &cmp).toMap();

            switch(token.req){
            case req_RangeMinValue:
                RangeMin = obj;
                emit rangeMinValueReceived(result);
                break;
            case req_RangeMaxValue:
                RangeMax = obj;
                emit rangeMaxValueReceived(result);
                break;
            case req_observe:
                if(code < 128){
                    qDebug() << "Start observing: " << uri;
                    emit observe_started(result, token.number);
                    disableTokenRemoval(token.number);
                    changeTokenRef(token.number, observe_monitor);
                }
                else{
                    emit observe_failed(token.number);
                    break;
                }
            [[clang::fallthrough]]; case observe_monitor:
            [[clang::fallthrough]]; case req_currentValue:
                LastValue = obj;
                emit currentValueChanged(token.number, result);
                valueUpdate(obj);
                break;
            case req_aboveEventValue:
                AboveEventAt = obj;
                emit aboveEventValueChanged(result);
                break;
            case req_belowEventValue:
                BelowEventAt = obj;
                emit belowEventValueChanged(result);
                break;
            case req_changeEventAt:
                ChangeEvent = obj;
                emit changeEventValueChanged(result);
                break;
            case req_getEventSetup:
                AboveEventAt = obj;
                if(!cmp_read_object(&cmp, &obj)) return QVariant(0);
                BelowEventAt = obj;
                if(!cmp_read_object(&cmp, &obj)) return QVariant(0);
                ChangeEvent = obj;
                if(!cmp_read_object(&cmp, &obj)) return QVariant(0);
                eventsActive = obj;
                emit eventSetupRdy();
                break;
            case req_updateEventsetup:
                qDebug() << "req_updateEventsetup";
                qDebug() << payload;
                break;
            case req_pairingslist:
                if(obj.type >= CMP_TYPE_BIN8 && obj.type <= CMP_TYPE_BIN32){
                    //First time we get here, clear the old pairingslist
                    //if(cont == 0) pairings->clear();
                    qDebug() << "req_pairingslist";
                    //cont = parsePairList(&cmp) == 0;
                }
                else{
                    qDebug() << "req_pairingslist - something in the message was wrong";
                }
                break;
            case req_clearparings:
                qDebug() << "req_clearparings";
                break;
            case req_removepairingitems:
                qDebug() << "req_clearparings";
                break;
            case req_pairsensor:
                if(obj.type == CMP_TYPE_UINT8){
                    //pairings->appended(token.number, result["value"].toInt());
                }
                break;
                //When we request a state change in the device, it always returns its current value
            case req_setCommand:
                LastValue = obj;
                valueUpdate(obj);
                emit currentValueChanged(token.number, result);
                qDebug() << "req_setCommand";
                break;
            case req_testevent:
                break;
            }
    }while(cmp.buf < payload.data() + payload.length() && cont);

    return QVariant(0);
}

pulsecounter::pulsecounter(node *parent, QString uri, QVariantMap attributes) : sensor(parent, uri, attributes){

}

/*************** Helpers ******************************************/

QVariant cmpobjectToVariant(cmp_object_t obj, cmp_ctx_t* cmp){
    QVariantMap result;
    int32_t size = 200;
    QByteArray str;
    QVariantList arr;
    result["enum_no"] = obj.type;
    unsigned long long temp;

    switch(obj.type){
    case CMP_TYPE_POSITIVE_FIXNUM:
        result["enum_str"] = "CMP_TYPE_POSITIVE_FIXNUM";
        result["value"] = obj.as.u8;
        break;
    case CMP_TYPE_NIL:
        result["enum_str"] = "CMP_TYPE_NIL";
        result["value"] = 0;
        break;
    case CMP_TYPE_UINT8:
        result["enum_str"] = "CMP_TYPE_UINT8";
        result["value"] = obj.as.u8;
        break;
    case CMP_TYPE_BOOLEAN:
        result["enum_str"] = "CMP_TYPE_BOOLEAN";
        break;
    case CMP_TYPE_FLOAT:
        result["enum_str"] = "CMP_TYPE_FLOAT";
        result["value"] = obj.as.flt;
        break;
    case CMP_TYPE_DOUBLE:
        result["enum_str"] = "CMP_TYPE_DOUBLE";
        result["value"] = obj.as.dbl;
        break;
    case CMP_TYPE_UINT16:
        result["enum_str"] = "CMP_TYPE_UINT16";
        result["value"] = obj.as.u16;
        break;
    case CMP_TYPE_UINT32:
        result["enum_str"] = "CMP_TYPE_UINT32";
        result["value"] = obj.as.u32;
        break;
    case CMP_TYPE_UINT64:
        result["enum_str"] = "CMP_TYPE_UINT64";
        temp = obj.as.u64;
        result["value"] = temp;
        break;
    case CMP_TYPE_SINT8:
        result["enum_str"] = "CMP_TYPE_SINT8";
        result["value"] = obj.as.s8;
        break;
    case CMP_TYPE_NEGATIVE_FIXNUM:
        result["enum_str"] = "CMP_TYPE_NEGATIVE_FIXNUM";
        result["value"] = obj.as.s8;
        break;
    case CMP_TYPE_SINT16:
        result["enum_str"] = "CMP_TYPE_SINT16";
        result["value"] = obj.as.s16;
        break;
    case CMP_TYPE_SINT32:
        result["enum_str"] = "CMP_TYPE_SINT32";
        result["value"] = obj.as.s32;
        break;
    case CMP_TYPE_SINT64:
        result["enum_str"] = "CMP_TYPE_SINT64";
        //result["value"] = Q_INT64_C(obj.as.s64);
        break;
    case CMP_TYPE_FIXMAP:
        result["enum_str"] = "CMP_TYPE_FIXMAP";
        break;
    case CMP_TYPE_FIXARRAY:
        result["enum_str"] = "CMP_TYPE_FIXARRAY";
        for(uint32_t i=0; i<obj.as.array_size; i++){
            //fixme: Only handles byte arrays
            int32_t s;
            cmp_read_int(cmp, &s);
            arr.append(s);
        }
        result["value"] = arr;
        break;
    case CMP_TYPE_FIXSTR:
        result["enum_str"] = "CMP_TYPE_FIXSTR";
        str.reserve(size);
        cmp->buf = static_cast<uint8_t*>(cmp->buf) - 1; //To read the string, we need to start at the size marker
        if(!cmp_read_str(cmp, str.data(), reinterpret_cast<uint32_t*>(&size))) return QVariant(0);
        str.resize(size);
        result["value"] = QString::fromLatin1(str.data());
        break;
    case CMP_TYPE_BIN8:
        result["enum_str"] = "CMP_TYPE_BIN8";
        str.reserve(size);
        cmp->buf = static_cast<uint8_t*>(cmp->buf) - 2;
        if(!cmp_read_bin(cmp, str.data(), reinterpret_cast<uint32_t*>(&size))) return QVariant(0);
        str.resize(size);
        result["value"] = str;
        break;
    case CMP_TYPE_BIN16:
        result["enum_str"] = "CMP_TYPE_BIN16";
        break;
    case CMP_TYPE_BIN32:
        result["enum_str"] = "CMP_TYPE_BIN32";
        break;
    case CMP_TYPE_EXT8:
        result["enum_str"] = "CMP_TYPE_EXT8";
        break;
    case CMP_TYPE_EXT16:
        result["enum_str"] = "CMP_TYPE_EXT16";
        break;
    case CMP_TYPE_EXT32:
        result["enum_str"] = "CMP_TYPE_EXT32";
        break;
    case CMP_TYPE_FIXEXT1:
        result["enum_str"] = "CMP_TYPE_FIXEXT1";
        break;
    case CMP_TYPE_FIXEXT2:
        result["enum_str"] = "CMP_TYPE_FIXEXT2";
        break;
    case CMP_TYPE_FIXEXT4:
        result["enum_str"] = "CMP_TYPE_FIXEXT4";
        break;
    case CMP_TYPE_FIXEXT8:
        result["enum_str"] = "CMP_TYPE_FIXEXT8";
        break;
    case CMP_TYPE_FIXEXT16:
        result["enum_str"] = "CMP_TYPE_FIXEXT16";
        break;
    case CMP_TYPE_STR8:
        result["enum_str"] = "CMP_TYPE_STR8";
        break;
    case CMP_TYPE_STR16:
        result["enum_str"] = "CMP_TYPE_STR16";
        break;
    case CMP_TYPE_STR32:
        result["enum_str"] = "CMP_TYPE_STR32";
        break;
    case CMP_TYPE_ARRAY16:
        result["enum_str"] = "CMP_TYPE_ARRAY16";
        break;
    case CMP_TYPE_ARRAY32:
        result["enum_str"] = "CMP_TYPE_ARRAY32";
        break;
    case CMP_TYPE_MAP16:
        result["enum_str"] = "CMP_TYPE_MAP16";
        break;
    case CMP_TYPE_MAP32:
        result["enum_str"] = "CMP_TYPE_MAP32";
        break;
    }
    return result;
}

cmp_object_t QVariantToCmpobject(uint8_t type, QVariant value){

    cmp_object_t obj;
    obj.type = type;

    switch(type){
    case CMP_TYPE_POSITIVE_FIXNUM:
    case CMP_TYPE_NIL:
    case CMP_TYPE_UINT8:
        obj.as.u8 = static_cast<uint8_t>(value.toUInt());
        break;
    case CMP_TYPE_BOOLEAN:
        obj.as.boolean = value.toBool();
        break;
    case CMP_TYPE_FLOAT:
        obj.as.flt = value.toFloat();
        break;
    case CMP_TYPE_DOUBLE:
        obj.as.dbl = value.toDouble();
        break;
    case CMP_TYPE_UINT16:
        obj.as.u16 = static_cast<uint16_t>(value.toUInt());
        break;
    case CMP_TYPE_UINT32:
        obj.as.u32 = value.toUInt();
        break;
    case CMP_TYPE_UINT64:
        obj.as.u64 = value.toUInt();
        break;
    case CMP_TYPE_SINT8:
    case CMP_TYPE_NEGATIVE_FIXNUM:
        obj.as.s8 = static_cast<int8_t>(value.toInt());
        break;
    case CMP_TYPE_SINT16:
        obj.as.s16 = static_cast<int16_t>(value.toInt());
        break;
    case CMP_TYPE_SINT32:
        obj.as.s32 = value.toInt();
        break;
    case CMP_TYPE_SINT64:
        obj.as.s64 = value.toInt();
        break;
    case CMP_TYPE_FIXMAP:
    case CMP_TYPE_FIXARRAY:
    case CMP_TYPE_FIXSTR:
    case CMP_TYPE_BIN8:
    case CMP_TYPE_BIN16:
    case CMP_TYPE_BIN32:
    case CMP_TYPE_EXT8:
    case CMP_TYPE_EXT16:
    case CMP_TYPE_EXT32:
    case CMP_TYPE_FIXEXT1:
    case CMP_TYPE_FIXEXT2:
    case CMP_TYPE_FIXEXT4:
    case CMP_TYPE_FIXEXT8:
    case CMP_TYPE_FIXEXT16:
    case CMP_TYPE_STR8:
    case CMP_TYPE_STR16:
    case CMP_TYPE_STR32:
    case CMP_TYPE_ARRAY16:
    case CMP_TYPE_ARRAY32:
    case CMP_TYPE_MAP16:
    case CMP_TYPE_MAP32:
        qDebug() << "QVariantToCmpobject: Type not yet implemented";
        break;
    }
    return obj;
}

/* Returns the len of the encoded message */
int encode(char* buffer, cmp_object_t objTemplate, QVariant value){
    cmp_ctx_t cmp;
    cmp_init(&cmp, buffer, nullptr, buf_writer);

    cmp_object_t obj = QVariantToCmpobject(objTemplate.type, value);

    cmp_write_object(&cmp, &obj);
    return static_cast<int>(static_cast<char*>(cmp.buf) - buffer);
}

bool buf_reader(cmp_ctx_t *ctx, void *data, uint32_t limit) {

    uint8_t* dataptr = static_cast<uint8_t*>(data);
    uint8_t* bufptr = static_cast<uint8_t*>(ctx->buf);

    for(uint32_t i=0; i<limit; i++){
        *dataptr++ = *bufptr++;
    }

    data = dataptr;
    ctx->buf = bufptr;

    return true;
}

static uint32_t buf_writer(cmp_ctx_t* ctx, const void *data, uint32_t count){

    uint8_t* dataptr = (uint8_t*)(data);
    uint8_t* bufptr = static_cast<uint8_t*>(ctx->buf);

    for(uint32_t i=0; i<count; i++){
        *bufptr++ = *dataptr++;
    }
    data = dataptr;
    ctx->buf = bufptr;

    return count;
}
