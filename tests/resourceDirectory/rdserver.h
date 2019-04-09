#ifndef RDSERVER_H
#define RDSERVER_H

#include <QObject>
#include <node.h>
#include <coap_server.h>
#include "node.h"

class rdServer;

class rd_res_core_well_known : public coap_resource
{
//    Q_OBJECT
public:
    rd_res_core_well_known(rdServer* parent);
    QByteArray getUri(){return "/.well-known/core";}
protected:


private:
    rdServer* parent;

    void handleGET(CoapPDU *request, CoapPDU *response, QByteArray* payload);
};

class rdServer : public QObject, coap_resource
{
    Q_OBJECT
public:
    explicit rdServer();

    void addNode(node* n);   
    node* getNodeRef(int index);

    QByteArray getUri(){ return "RM/"; }

private:
    rd_res_core_well_known* core;
    QVector<coap_resource*> resources;
    QVector<node*> nodes;

    void handleGET(CoapPDU *request, CoapPDU *response, QByteArray* payload);
    void handlePOST(CoapPDU *request, CoapPDU *response);
    void handlePUT(CoapPDU *request, CoapPDU *response);
    void handleDELETE(CoapPDU *request, CoapPDU *response);

signals:

public slots:
};

#endif // RDSERVER_H
