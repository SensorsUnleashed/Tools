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
//Created from QML
node::node(QHostAddress addr) : suinterface(addr)
{
    ip = addr;
    prefix_len = 64; //This should be taken from somewhere in the database
    uri = "su/nodeinfo";
    m_commStatus = UNKNOWN;
    qDebug() << "Node: " << ip << " created";
}

/* Request the list of sensors from the node */
void node::requestLinks(){

    msgid t;
    token = qrand();
    t.number = token;

    const char* uristring = ".well-known/core";

    CoapPDU *pdu = new CoapPDU();
    pdu->setType(CoapPDU::COAP_CONFIRMABLE);
    pdu->setCode(CoapPDU::COAP_GET);
    pdu->setToken((uint8_t*)&token,2);

    enum CoapPDU::ContentFormat ct = CoapPDU::COAP_CONTENT_FORMAT_APP_LINK;
    pdu->addOption(CoapPDU::COAP_OPTION_CONTENT_FORMAT,1,(uint8_t*)&ct);
    pdu->setMessageID(token);
    pdu->setURI((char*)uristring, strlen(uristring));

    send(pdu, t);
}

/* Parse the list of published sensors from this node */
QVariant node::parseAppLinkFormat(msgid token, QByteArray payload){
    Q_UNUSED(token);
    qDebug() << "node: parseAppLinkFormat";

    QString pl = QString(payload);
    //All resources are separeted by a ','
    QStringList rlist = pl.split(',', QString::SkipEmptyParts);
    for(int i=0; i<rlist.count(); i++){
        QStringList slist = rlist.at(i).split(';', QString::SkipEmptyParts);
        QString uri;
        QVariantMap attributes;

        for(int j=0; j<slist.count(); j++){
            if(j==0){   //Uri
                //For now index 0 is always the uri
                uri = slist.at(0);
                uri.remove(QRegExp("[<>]"));
                if( uri.at(0) == '/' ) uri.remove( 0, 1 );    //Remove leading "/"
                //slist.removeAt(0);
            }
            else{   //Attributes as key value pairs
                QStringList keyval = slist.at(j).split("=");
                if(keyval.size() == 2){
                    attributes[keyval[0]] = keyval[1];
                }
            }
        }
        //addSensor(uri, attributes);
    }
    this->token = 0;
    return QVariant(0);
}

//void node::addSensor(QString uri, QVariantMap attributes){
//    if(uri.compare("su/nodeinfo") == 0){
//        //This is a special device, we use to handle special commands, like
//        //Format the filesystem, factory reset, hw/sw version request. etc
//        //Its always there, so we have already created this device during node
//        //creation
//    }
//    else if(uri.compare(".well-known/core") != 0){
//        sensor* s;
//        if(uri.compare("su/pulsecounter") == 0){
//            s = new pulsecounter(this, uri, attributes, allsensorslist);
//        }
//        else if(uri.compare("su/timer") == 0){
//            s = new timerdevice(this, uri, attributes, allsensorslist);
//        }
//        else if(
//                uri.compare("su/powerrelay") == 0 ||
//                uri.compare("su/ledindicator") == 0 ||
//                uri.compare("su/led_yellow") == 0 ||
//                uri.compare("su/led_red") == 0 ||
//                uri.compare("su/led_orange") == 0 ||
//                uri.compare("su/led_green") == 0
//                ){
//            s = new defaultdevice(this, uri, attributes, allsensorslist);
//        }
//        else{
//            s = new sensor(this, uri, attributes, allsensorslist);
//        }
//        emit sensorFound(uri, s->getClassType());
//    }
//}

QVariant node::request_cfs_format(){
    const char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI((char*)uristring, strlen(uristring));
    pdu->addURIQuery((char*)"cfsformat");

    return put_request(pdu, format_filesystem);
}

QVariant node::request_observe_retry(){
    const char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI((char*)uristring, strlen(uristring));
    pdu->addURIQuery((char*)"obsretry");

    return put_request(pdu, observe_retry);
}

QVariant node::request_versions(){
    const char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI((char*)uristring, strlen(uristring));
    pdu->addURIQuery((char*)"Versions");

    return get_request(pdu, req_versions);
}

QVariant node::request_SlotNfo(int active){
    char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(uristring, strlen(uristring));

    if(active){
        pdu->addURIQuery("SlotNfo=1");
        return get_request(pdu, req_slotNfoActive);
    }

    pdu->addURIQuery((char*)"SlotNfo=0");
    return get_request(pdu, req_slotNfoBackup);
}

QVariant node::request_active_slot(){
    char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(uristring, strlen(uristring));

    pdu->addURIQuery((char*)"activeSlot");
    return get_request(pdu, req_activeslot);
}


QVariant node::request_coapstatus(){
    const char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI((char*)uristring, strlen(uristring));
    pdu->addURIQuery((char*)"CoapStatus");

    return get_request(pdu, req_coapstatus);
}

QVariant node::request_swreset(){
    const char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI((char*)uristring, strlen(uristring));
    pdu->addURIQuery((char*)"swreset");

    return put_request(pdu, req_swreset);
}

#include <QFile>
#include <QUrl>
QVariant node::swupgrade(QString filename){
    qDebug() << "Upgrade using " << filename;

    QByteArray binary;

    QFile file(QUrl(filename).path())   ;

    if (!file.open(QIODevice::ReadOnly))
        return 0;

    if(file.size() > (512*1024)) return 0;

    while (!file.atEnd()) {
        binary.append(file.readLine());
    }

    char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(uristring, strlen(uristring));
    pdu->addURIQuery((char*)"upg");

    return put_request(pdu, sw_upgrade, binary, 1);
}


void node::handleReturnCode(msgid token, CoapPDU::Code code){
    Q_UNUSED(token);
    Q_UNUSED(code);
    qDebug() << "Got token again";

    if(code == CoapPDU::COAP_NOT_FOUND){

    }

}

QVariant addDateTime(uint32_t epoch){
    const QDateTime dt = QDateTime::fromTime_t(epoch);

    return dt;
}

void node::nodeNotResponding(msgid token){
    qDebug() << "Node: " << getAddressStr() << " token: " << token.number;
    setCommStatus(NOTOK);
}

void node::nodeResponding(msgid token){
    qDebug() << "Node: " << getAddressStr() << " token: " << token.number;
    setCommStatus(OK);
}


QVariant node::parseAppOctetFormat(msgid token, QByteArray payload, CoapPDU::Code code) {
    Q_UNUSED(code);
    qDebug() << uri << " got message!";
    cmp_object_t obj;
    cmp_ctx_t cmp;

    cmp_init(&cmp, payload.data(), buf_reader, nullptr);

    QVariantList res;
    while(cmp.buf < payload.data() + payload.length()){
        if(!cmp_read_object(&cmp, &obj)) return QVariant(0);

        QVariantMap result = cmpobjectToVariant(obj, &cmp).toMap();
        res.append(result);
    }

    switch(token.req){
    case req_versions:
        emit requst_received("req_versions", res);
        break;
    case req_coapstatus:
        emit requst_received("req_coapstatus", res);
        break;
    case req_slotNfoActive:
        res.append(addDateTime(res[2].toMap()["value"].toULongLong()));
        emit requst_received("req_slotNfoActive", res);
        break;
    case req_slotNfoBackup:
        res.append(addDateTime(res[2].toMap()["value"].toULongLong()));
        emit requst_received("req_slotNfoBackup", res);
        break;
    case req_activeslot:
        emit requst_received("req_activeslot", res);
        break;
    default:
        break;
    }

    return QVariant(0);
}

