#ifndef COAP_SERVER_H
#define COAP_SERVER_H

#include <QObject>
#include <QDebug>
#include <QHostAddress>
#include "cantcoap/cantcoap.h"
#include <coap_resource.h>
#include <coap_observer.h>

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
    coap_server(quint16 port = DEFAULT_COAP_PORT);
    virtual ~coap_server();

    coap_transaction* getHandler(CoapPDU *recvPDU, QHostAddress addr, quint16 port);
    coap_transaction* postHandler(CoapPDU *recvPDU, QHostAddress addr, quint16 port);
    coap_transaction* putHandler(CoapPDU *recvPDU, QHostAddress addr, quint16 port);
    coap_transaction* deleteHandler(CoapPDU *recvPDU, QHostAddress addr, quint16 port);
    coap_resource* findResource(CoapPDU *PDU);

    void add_observer(coap_observer* c, QHostAddress addr, quint16 port);
    void addResource(coap_resource* resource);
    coap_resource* getResource(int index);
    void handleObservers(coap_resource* res);
    void receiverNotResponding(QHostAddress addr, quint16 port);
private:

    QVector<coap_resource*> resources;
    QVector<coap_observer*> observers;

    res_core_well_known* core;
    void remove_observer_by_addr(QHostAddress addr, quint16 port);
    void remove_observer_by_uri(coap_resource* res, QHostAddress addr, quint16 port);

protected:
    virtual void handleGET(CoapPDU *request, CoapPDU *response, QByteArray* payload);
    virtual void handlePOST(CoapPDU *request, CoapPDU *response);
    virtual void handlePUT(CoapPDU *request, CoapPDU *response);
    virtual void handleDELETE(CoapPDU *request, CoapPDU *response);

    virtual void observerLost(QHostAddress addr, quint16 port) { Q_UNUSED(addr); Q_UNUSED(port);}
    virtual void observerAdded(QHostAddress addr, quint16 port) { Q_UNUSED(addr); Q_UNUSED(port);}
};

#endif // COAP_SERVER_H
