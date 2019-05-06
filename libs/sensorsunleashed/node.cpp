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
#include "coap_transaction.h"
#include <QRandomGenerator>
#include "su_message.h"

//Created from QML
node::node(QHostAddress addr, quint16 port) : suinterface(addr, port)
{
    prefix_len = 64; //This should be taken from somewhere in the database
    uri = "su/nodeinfo";
    m_commStatus = UNKNOWN;
    qDebug() << "Node: " << addr << " created";
}

/* Request the list of sensors from the node */
void node::requestLinks(){
    quint32 t = QRandomGenerator::global()->generate();

    QByteArray token;

    token.append(static_cast<char>(t));
    token.append(static_cast<char>(t >> 8));
    token.append(static_cast<char>(t >> 16));
    token.append(static_cast<char>(t >> 24));

    const char* uristring = ".well-known/core";

    CoapPDU *pdu = new CoapPDU();
    pdu->setType(CoapPDU::COAP_CONFIRMABLE);
    pdu->setCode(CoapPDU::COAP_GET);
    pdu->setToken(reinterpret_cast<uint8_t*>(token.data()), static_cast<uint8_t>(token.length()));

    enum CoapPDU::ContentFormat ct = CoapPDU::COAP_CONTENT_FORMAT_APP_LINK;
    pdu->addOption(CoapPDU::COAP_OPTION_CONTENT_FORMAT,1,reinterpret_cast<uint8_t*>(&ct));
    pdu->setMessageID(1);
    pdu->setURI(const_cast<char*>(uristring), strlen(uristring));

    new coap_client_transaction(addr, port, pdu, this, nullptr);
}

/* Parse the list of published sensors from this node */
QVariant node::parseAppLinkFormat(QByteArray token, QByteArray payload){
    Q_UNUSED(token);
    coreLinks = payload;
    qDebug() << "node: parseAppLinkFormat";


    QString pl = QString(payload);
    //All resources are separeted by a ','
    QStringList rlist = pl.split(',', QString::SkipEmptyParts);
    for(int i=0; i<rlist.count(); i++){
        coap_resource* r = new coap_resource(rlist.at(i));
        addSensor(r);
    }
    emit linkParsingDone();
    this->token = 0;
    return QVariant(0);
}

void node::addSensor(coap_resource* resource){

    QString uri = resource->getUri();

    if(uri.compare("su/nodeinfo") == 0){
        //This is a special device, we use to handle special commands, like
        //Format the filesystem, factory reset, hw/sw version request. etc
        //Its always there, so we have already created this device during node
        //creation
    }
    else if(uri.compare(".well-known/core") != 0){

        for(int i=0; i<sensors.count(); i++){
            if(QString(sensors[i]->getUri()).compare(uri) == 0){
                return;
            }
        }

        sensor* s;
        if(uri.compare("su/pulsecounter") == 0){
            s = new pulsecounter(this, resource);
        }
        else if(uri.compare("su/timer") == 0){
            s = new timerdevice(this, resource);
        }
        else if(
                uri.compare("su/powerrelay") == 0 ||
                uri.compare("su/ledindicator") == 0 ||
                uri.compare("su/led_yellow") == 0 ||
                uri.compare("su/led_red") == 0 ||
                uri.compare("su/led_orange") == 0 ||
                uri.compare("su/led_green") == 0
                ){
            s = new defaultdevice(this, resource);
        }
        else{
            s = new sensor(this, resource);
        }

        sensors.append(s);
        emit sensorCreated(s);
        emit sensorFound(uri, s->getClassType());
    }
}

QVariant node::request_cfs_format(){
    const char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(const_cast<char*>(uristring), strlen(uristring));
    pdu->addURIQuery(const_cast<char*>("cfsformat"));

    return put_request(pdu, su_message::format_filesystem);
}

QVariant node::request_observe_retry(){
    const char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(const_cast<char*>(uristring), strlen(uristring));
    pdu->addURIQuery(const_cast<char*>("obsretry"));

    return put_request(pdu, su_message::observe_retry);
}

QVariant node::request_versions(){
    const char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(const_cast<char*>(uristring), strlen(uristring));
    pdu->addURIQuery(const_cast<char*>("Versions"));

    return get_request(pdu, su_message::req_versions);
}

QVariant node::request_SlotNfo(int active){
    char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(uristring, strlen(uristring));

    if(active){
        pdu->addURIQuery("SlotNfo=1");
        return get_request(pdu, su_message::req_slotNfoActive);
    }

    pdu->addURIQuery(const_cast<char*>("SlotNfo=0"));
    return get_request(pdu, su_message::req_slotNfoBackup);
}

QVariant node::request_active_slot(){
    char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(uristring, strlen(uristring));

    pdu->addURIQuery(const_cast<char*>("activeSlot"));
    return get_request(pdu, su_message::req_activeslot);
}


QVariant node::request_coapstatus(){
    const char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(const_cast<char*>(uristring), strlen(uristring));
    pdu->addURIQuery(const_cast<char*>("CoapStatus"));

    return get_request(pdu, su_message::req_coapstatus);
}

QVariant node::request_swreset(){
    const char* uristring = uri.toLatin1().data();
    CoapPDU *pdu = new CoapPDU();
    pdu->setURI(const_cast<char*>(uristring), strlen(uristring));
    pdu->addURIQuery(const_cast<char*>("swreset"));

    return put_request(pdu, su_message::req_swreset);
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
    pdu->addURIQuery(const_cast<char*>("upg"));

    return put_request(pdu, su_message::sw_upgrade, binary, 1);
}


void node::handleReturnCode(QByteArray token, CoapPDU::Code code){
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

void node::nodeNotResponding(QByteArray token){
    qDebug() << "Node: " << getAddressStr() << " token: " << token;
    setCommStatus(NOTOK);
}

void node::nodeResponding(QByteArray token){
    qDebug() << "Node: " << getAddressStr() << " token: " << token;
    setCommStatus(OK);
}


QVariant node::parseAppOctetFormat(QByteArray token, QByteArray payload, CoapPDU::Code code) {
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

    int req = getTokenref(token);

    switch(req){
    case su_message::req_versions:
        emit requst_received("req_versions", res);
        break;
    case su_message::req_coapstatus:
        emit requst_received("req_coapstatus", res);
        break;
    case su_message::req_slotNfoActive:
        res.append(addDateTime(res[2].toMap()["value"].toULongLong()));
        emit requst_received("req_slotNfoActive", res);
        break;
    case su_message::req_slotNfoBackup:
        res.append(addDateTime(res[2].toMap()["value"].toULongLong()));
        emit requst_received("req_slotNfoBackup", res);
        break;
    case su_message::req_activeslot:
        emit requst_received("req_activeslot", res);
        break;
    default:
        break;
    }

    return QVariant(0);
}

