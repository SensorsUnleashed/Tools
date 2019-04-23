#ifndef COAP_SERVER_H
#define COAP_SERVER_H

#include <QObject>
#include <QDebug>
#include <QHostAddress>
#include "cantcoap/cantcoap.h"
#include <coap_resource.h>
#include <coap_observer.h>
//#include "coap_transaction.h"
class coap_transaction;
class coap_server;

class res_core_well_known : public coap_resource
{
//    Q_OBJECT
public:
    res_core_well_known(coap_server* parent);
    QByteArray getUri(){return "/.well-known/core";}


protected:


private:
    coap_server* parent;

    void handleGET(CoapPDU *request, CoapPDU *response, QByteArray* payload);
};

class coap_server : public QObject
{
    Q_OBJECT

public:
    coap_server();
    virtual ~coap_server();

    coap_transaction *handleRequest(CoapPDU *recvPDU, QHostAddress addr, quint16 port);
    void addResource(coap_resource* resource);
    coap_resource* getResource(int index);
    void handleObservers(coap_resource* res);
private:

    QVector<coap_resource*> resources;
    QVector<coap_observer*> observers;

    res_core_well_known* core;
    void remove_observer_by_uri(coap_resource* res, QHostAddress addr, quint16 port);

protected:
    virtual void handleGET(CoapPDU *request, CoapPDU *response, QByteArray* payload);
    virtual void handlePOST(CoapPDU *request, CoapPDU *response);
    virtual void handlePUT(CoapPDU *request, CoapPDU *response);
    virtual void handleDELETE(CoapPDU *request, CoapPDU *response);
};

#endif // COAP_SERVER_H
