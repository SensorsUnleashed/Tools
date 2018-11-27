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
#ifndef NODE_H
#define NODE_H

#include "sensorsunleashed_global.h"

#include <QObject>

#include <QHostAddress>
#include "cmp_helpers.h"
#include "wsn.h"
#include <QDateTime>

class pairlist;
class sensorstore;
class node;

int findToken(uint16_t token, QVector<msgid> tokenlist);
QVariant cmpobjectToVariant(cmp_object_t obj, cmp_ctx_t *cmp = nullptr);
bool buf_reader(cmp_ctx_t *ctx, void *data, uint32_t limit);

class SENSORSUNLEASHEDSHARED_EXPORT suinterface : public wsn
{
    Q_OBJECT
public:
    suinterface(QHostAddress addr);

protected:
    quint16 get_request(CoapPDU *pdu, enum request req, QByteArray payload=nullptr, quint8 allow_retry=0);
    quint16 put_request(CoapPDU *pdu, enum request req, QByteArray payload=nullptr, quint8 allow_retry=0);

private:

};

class SENSORSUNLEASHEDSHARED_EXPORT sensor : public suinterface
{
    Q_OBJECT
public:
    sensor(node* parent, QString uri, QVariantMap attributes);

    //Dummy constructor
    sensor(QString ipaddr, QString uri);

    node* getParent(){ return parent;}
    QString getUri(){ return uri; }
    QString getAddressStr() {return ip.toString(); }

    void initSensor();
    void requestRangeMin();
    void requestRangeMax();

    void requestValue();
    QVariant requestObserve();
    void abortObserve(QVariant token);
    void requestAboveEventLvl();
    void requestBelowEventLvl();
    void requestChangeEventLvl();

    void req_eventSetup();
    void updateConfig(QVariant updatevalues);
    void flashSave();
    QVariant getConfigValues();   //Get last stored values without quering the sensor

    /* Pair this sensor with another. */
    void getpairingslist();
    QVariant clearpairingslist();
    uint16_t removeItems(QByteArray arr);
    QVariant pair(QVariant pairdata);

    void testEvents(QVariant event, QVariant value);

    void handleReturnCode(msgid token, CoapPDU::Code code);
    void nodeNotResponding(msgid token);
    QVariant parseAppOctetFormat(msgid token, QByteArray payload, CoapPDU::Code code);

    virtual QVariant getClassType(){ return "DefaultSensor.qml"; }
    virtual QVariant getActionModel() { return "DefaultActions.qml"; }

protected:
    QString uri;

    //uint16_t put_request(CoapPDU *pdu, enum request req, QByteArray payload);
private:
    node* parent;
    QVariantMap sensorinfo;
    //    QVector<msgid> token;
    pairlist* pairings;
    QHostAddress ip;
    uint8_t init;   //Flag to indicate if sensor config has been requested or not

    cmp_object_t eventsActive;		//All events on or Off
    cmp_object_t LastValue;
    cmp_object_t AboveEventAt;	//When resource crosses this line from low to high give an event (>=)
    cmp_object_t BelowEventAt;	//When resource crosses this line from high to low give an event (<=)
    cmp_object_t ChangeEvent;	//When value has changed more than changeEvent + lastevent value <>= value
    cmp_object_t RangeMin;		//What is the minimum value this device can read
    cmp_object_t RangeMax;		//What is the maximum value this device can read

    //uint16_t get_request(CoapPDU *pdu, enum request req, QByteArray payload=0);

signals:
    void currentValueChanged(QVariant result);
    void observe_started(QVariant result, uint16_t token);
    void observe_failed(uint16_t token);
    void aboveEventValueChanged(QVariant result);
    void belowEventValueChanged(QVariant result);
    void changeEventValueChanged(QVariant result);

    void eventSetupRdy();

    void rangeMaxValueReceived(QVariant result);
    void rangeMinValueReceived(QVariant result);
};

class SENSORSUNLEASHEDSHARED_EXPORT timerdevice : public sensor {
    Q_OBJECT
public:
    timerdevice(node* parent, QString uri, QVariantMap attributes, sensorstore *p);
    QVariant getClassType(){ return "TimerDevice.qml"; }

private:
private slots:

};

#include <QDate>
class SENSORSUNLEASHEDSHARED_EXPORT defaultdevice : public sensor {
    Q_OBJECT

public:
    defaultdevice(node *parent, QString uri, QVariantMap attributes, sensorstore *p);

    QVariant getClassType(){ return "DefaultDevice.qml"; }

    void setToggle();
    void setOn();
    void setOff();
};

class SENSORSUNLEASHEDSHARED_EXPORT node : public suinterface
{
    Q_OBJECT
    Q_PROPERTY(commStatus commStatus READ getCommStatus NOTIFY commStatusChanged)
public:
    node(QHostAddress addr);

    QVariant getDatabaseinfo(){ return databaseinfo; }
    QHostAddress getAddress() { return ip; }
    Q_INVOKABLE QString getAddressStr() {return ip.toString(); }
    void requestLinks();

    void updateLastSeenTime(int secSince){
        lastSeen = QDateTime::currentDateTime().addSecs(-secSince);
    }
    QDateTime getLastSeenTime() { return lastSeen; }

    /* Virtual functions (wsn)*/
    QVariant parseAppLinkFormat(msgid token, QByteArray payload);
    QVariant parseAppOctetFormat(msgid token, QByteArray payload);

    uint8_t getPrefixlen(){ return prefix_len; }

    Q_INVOKABLE QVariant request_cfs_format();
    Q_INVOKABLE QVariant request_observe_retry();
    Q_INVOKABLE QVariant request_versions();
    Q_INVOKABLE QVariant request_SlotNfo(int active);
    Q_INVOKABLE QVariant request_active_slot();
    Q_INVOKABLE QVariant request_coapstatus();
    Q_INVOKABLE QVariant request_swreset();
    Q_INVOKABLE QVariant swupgrade(QString filename);

    void handleReturnCode(msgid token, CoapPDU::Code code);
    void nodeNotResponding(msgid token);
    void nodeResponding(msgid token);

    QVariant parseAppOctetFormat(msgid token, QByteArray payload, CoapPDU::Code code);

    enum commStatus { OK, NOTOK, UNKNOWN };
    Q_ENUMS(commStatus)

    commStatus getCommStatus(){ return m_commStatus;}
    void setCommStatus(commStatus status){
        m_commStatus = status;
        emit commStatusChanged();
    }

protected:
    QString uri;
    commStatus m_commStatus;

private:
    QString name;
    QHostAddress ip;
    QVariantMap linklist;
    uint16_t token;
    QDateTime lastSeen;
    QVariantMap databaseinfo;
    QVector<sensor*> sensors;

    uint8_t prefix_len;

signals:
    void sensorFound(QVariant sensorinfo, QVariant source);
    void requst_received(QString req, QVariantList result);
    void commStatusChanged();
public slots:
};


#endif // NODE_H
