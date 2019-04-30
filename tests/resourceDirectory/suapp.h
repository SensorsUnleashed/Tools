#ifndef SUAPP_H
#define SUAPP_H
#include <QObject>
#include <QHash>
#include <node.h>
#include "rdserver.h"
#include <coap_resource.h>

class su_resource : public coap_resource
{
public:
    su_resource(sensor* s) : coap_resource(){
        this->s = s;
    }

    virtual ~su_resource(){

    }

private:
    sensor* s;

    QByteArray getUri(){
        QString ip = s->getAddress().toString();
        return "/" + ip.toLatin1() + "/" + s->getUri();
    }

    void handleGET(CoapPDU *request, CoapPDU *response, QByteArray *payload);
    void handlePOST(CoapPDU *request, CoapPDU *response){ Q_UNUSED(request); Q_UNUSED(response); qDebug() << "Implement handlePOST";}
    void handlePUT(CoapPDU *request, CoapPDU *response){ Q_UNUSED(request); Q_UNUSED(response); qDebug() << "Implement handlePUT";}
    void handleDELETE(CoapPDU *request, CoapPDU *response){ Q_UNUSED(request); Q_UNUSED(response); qDebug() << "Implement handleDELETE";}
};

class suapp : public coap_server
{
    Q_OBJECT
public:
    suapp();
    virtual ~suapp();
private:
    rdServer* rd;
    QHash<suValue*, coap_resource*> observees;
public slots:
    void sensorCreated(sensor* s);

private slots:
    void sensorValueChanged();
};

#endif // SUAPP_H
