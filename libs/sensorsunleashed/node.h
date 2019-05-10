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
#include <QDateTime>
#include <coap_resource.h>
#include <messagepack.h>

class pairlist;
class sensorstore;
class node;

//int findToken(uint16_t token, QVector<msgid> tokenlist);
QVariant cmpobjectToVariant(cmp_object_t obj, cmp_ctx_t *cmp = nullptr);
bool buf_reader(cmp_ctx_t *ctx, void *data, uint32_t limit);

class SENSORSUNLEASHEDSHARED_EXPORT suinterface : public QObject
{
    Q_OBJECT
public:
    suinterface(QHostAddress addr, quint16 port);

    QByteArray request(CoapPDU *pdu, int req, QByteArray payload = nullptr);

    void parseMessage(QByteArray token, QByteArray message, CoapPDU::Code code, enum CoapPDU::ContentFormat ct);
    virtual void noResponse() {}

    QHostAddress getAddress() { return addr; }
    quint16 getPort() { return port; }
    Q_INVOKABLE QString getAddressStr() {return addr.toString(); }

private:

protected:
    QHostAddress addr;
    quint16 port;

    QByteArray get_request(CoapPDU *pdu, int req, QByteArray payload=nullptr, quint8 allow_retry=0);
    QByteArray put_request(CoapPDU *pdu, int req, QByteArray payload=nullptr, quint8 allow_retry=0);

    int getTokenref(QByteArray token);
    void setTokenref(QByteArray token, int ref);

    virtual QVariant parseTextPlainFormat(QByteArray token, QByteArray payload){ qDebug() << "wsn::parseTextPlainFormat " << payload << " token=" << token; return QVariant(0);}
    virtual QVariant parseAppLinkFormat(QByteArray token, QByteArray payload) { Q_UNUSED(payload); Q_UNUSED(token); qDebug() << "wsn::parseAppLinkFormat Implement this"; return QVariant(0);}
    virtual QVariant parseAppXmlFormat(QByteArray token, QByteArray payload) { Q_UNUSED(payload); Q_UNUSED(token); qDebug() << "wsn::parseAppXmlFormat Implement this"; return QVariant(0);}
    virtual QVariant parseAppOctetFormat(QByteArray token, QByteArray payload, CoapPDU::Code code) { Q_UNUSED(payload); Q_UNUSED(token); Q_UNUSED(code); qDebug() << "wsn::parseAppOctetFormat Implement this"; return QVariant(0);}
    virtual QVariant parseAppExiFormat(QByteArray token, QByteArray payload) { Q_UNUSED(payload); Q_UNUSED(token); qDebug() << "wsn::parseAppExiFormat Implement this"; return QVariant(0);}
    virtual QVariant parseAppJSonFormat(QByteArray token, QByteArray payload) { Q_UNUSED(payload); Q_UNUSED(token); qDebug() << "wsn::parseAppJSonFormat Implement this"; return QVariant(0);}
    virtual void handleError(QByteArray token, QByteArray message, CoapPDU::Code code, enum CoapPDU::ContentFormat ct);

private:
    QHash<QByteArray, int> tokenref;
};

class SENSORSUNLEASHEDSHARED_EXPORT sensor : public suinterface
{
    Q_OBJECT
public:
    sensor(node* parent, coap_resource* resource);

    //Dummy constructor
    sensor(QString ipaddr, QString uri);

    node* getParent(){ return parent;}
    QByteArray getUri(){ return resource->getUri(); }
    int getID(){ return resource->getID();}
    void setID(int id){ resource->setID(id);}
    int getType(){ return deviceType; }

    void initSensor();
    void requestRangeMin();
    void requestRangeMax();

    void requestValue(const char *query = nullptr);

    QVariant requestObserve(QString event);
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
    QByteArray removeItems(QByteArray arr);
    QVariant pair(QVariant pairdata);

    void testEvents(QVariant event, QVariant value);

    void handleReturnCode(QByteArray token, CoapPDU::Code code);
    void nodeNotResponding(QByteArray token);
    QVariant parseAppOctetFormat(QByteArray token, QByteArray payload, CoapPDU::Code code);

    virtual QVariant getClassType(){ return "DefaultSensor.qml"; }
    virtual QVariant getActionModel() { return "DefaultActions.qml"; }

    virtual void valueUpdate(cmp_object_t) {}
    virtual int8_t getValueType(){ return CMP_TYPE_UINT8; }

    suValue* getnLastValue(){ return nLastValue; }
    suValue* getMaxLimit(){ return nRangeMax; }
    suValue* getMinLimit(){ return nRangeMin; }

    enum DeviceType{
        pulseCounter = 1,
        timer,
        pushbutton,
        ledIndicator,
        powerRelay,

        unkownDeviceType = 0xff,
    };

protected:

    suValue* neventsActive = nullptr;		//All events on or Off
    suValue* nLastValue = nullptr;
    suValue* nAboveEventAt = nullptr;	//When resource crosses this line from low to high give an event (>=)
    suValue* nBelowEventAt = nullptr;	//When resource crosses this line from high to low give an event (<=)
    suValue* nChangeEvent = nullptr;	//When value has changed more than changeEvent + lastevent value <>= value
    suValue* nRangeMin = nullptr;	//What is the minimum value this device can read
    suValue* nRangeMax = nullptr;	//What is the maximum value this device can read

    int deviceType = unkownDeviceType;

private:
    node* parent;
    QVariantMap sensorinfo;
    pairlist* pairings;
    uint8_t init;   //Flag to indicate if sensor config has been requested or not
    coap_resource* resource;

signals:
    void currentValueChanged(QByteArray token, QVariant result);
    void observe_started(QVariant result, QByteArray token);
    void observe_failed(QByteArray token);
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
    timerdevice(node* parent, coap_resource* resource) : sensor(parent, resource){
        deviceType = timer;
    }

    QVariant getClassType(){ return "TimerDevice.qml"; }

private:
private slots:

};

#include <QDate>
class SENSORSUNLEASHEDSHARED_EXPORT defaultdevice : public sensor {
    Q_OBJECT

public:
    defaultdevice(node *parent, coap_resource* resource, enum DeviceType type) : sensor(parent, resource){
        deviceType = type;
    }

    QVariant getClassType(){ return "DefaultDevice.qml"; }

    void setToggle();
    void setOn();
    void setOff();

private:
};

class SENSORSUNLEASHEDSHARED_EXPORT pulsecounter : public sensor {
    Q_OBJECT

public:
    pulsecounter(node *parent, coap_resource *resource) : sensor(parent, resource){
        deviceType = pulseCounter;
    }

    QVariant getClassType(){ return "DefaultDevice.qml"; }
    int8_t getValueType(){ return CMP_TYPE_UINT16; }

protected:


};

class SENSORSUNLEASHEDSHARED_EXPORT node : public suinterface
{
    Q_OBJECT
    Q_PROPERTY(commStatus commStatus READ getCommStatus NOTIFY commStatusChanged)
public:
    node(QHostAddress addr, quint16 port = 5683);

    void setID(int id){ this->id = id; }
    int getID(){ return id; }

    QVariant getDatabaseinfo(){ return databaseinfo; }
    void requestLinks();

    void updateLastSeenTime(int secSince){
        lastSeen = QDateTime::currentDateTime().addSecs(-secSince);
    }
    QDateTime getLastSeenTime() { return lastSeen; }

    /* Virtual functions (wsn)*/
    QVariant parseAppLinkFormat(QByteArray token, QByteArray payload);
    QVariant parseAppOctetFormat(QByteArray token, QByteArray payload);

    uint8_t getPrefixlen(){ return prefix_len; }

    Q_INVOKABLE QVariant request_cfs_format();
    Q_INVOKABLE QVariant request_observe_retry();
    Q_INVOKABLE QVariant request_versions();
    Q_INVOKABLE QVariant request_SlotNfo(int active);
    Q_INVOKABLE QVariant request_active_slot();
    Q_INVOKABLE QVariant request_coapstatus();
    Q_INVOKABLE QVariant request_swreset();
    Q_INVOKABLE QVariant swupgrade(QString filename);

    void handleReturnCode(QByteArray token, CoapPDU::Code code);
    void nodeNotResponding(QByteArray token);
    void nodeResponding(QByteArray token);

    QVariant parseAppOctetFormat(QByteArray token, QByteArray payload, CoapPDU::Code code);

    enum commStatus { OK, NOTOK, UNKNOWN };
    Q_ENUMS(commStatus)

    commStatus getCommStatus(){ return m_commStatus;}
    void setCommStatus(commStatus status){
        m_commStatus = status;
        emit commStatusChanged();
    }

    void addSensor(coap_resource* resource);

protected:
    QString uri;
    commStatus m_commStatus;

private:
    QString name;
    QVariantMap linklist;
    uint16_t token;
    QDateTime lastSeen;
    QVariantMap databaseinfo;
    QVector<sensor*> sensors;
    int id = -1;

    uint8_t prefix_len;

    QByteArray coreLinks;

signals:
    void sensorCreated(sensor*);
    void sensorFound(QVariant sensorinfo, QVariant source);
    void linkParsingDone();
    void requst_received(QString req, QVariantList result);
    void commStatusChanged();
public slots:
};


#endif // NODE_H
