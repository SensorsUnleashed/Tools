#ifndef SUAPP_H
#define SUAPP_H
#include <QObject>
#include <QHash>
#include <node.h>
#include <coap_server.h>
#include <coap_resource.h>
#include "config.h"

class su_resource;
class suapp : public coap_server
{
    Q_OBJECT
public:
    suapp(config* configuration);
    virtual ~suapp();

    void observerLost(QHostAddress addr, quint16 port);
    void observerAdded(QHostAddress addr, quint16 port);

private:
    config* configuration;
    QHash<suValue*, su_resource*> observees;
    QVector<obsdevice*>* observerList;
    int nodeDataPending = 0;

    void notifyLost();
    void addNode(node* n);

private slots:
    void linksParsed();

public slots:
    void sensorCreated(sensor* s);

private slots:
    void sensorValueChanged();
};

class su_resource : public coap_resource
{
public:
    su_resource(sensor* s) : coap_resource(){
        this->s = s;
    }

    virtual ~su_resource(){

    }

    sensor* getSensor(){ return s; }

private:
    sensor* s = nullptr;

    QByteArray getUri(){
        QString ip = s->getAddress().toString();
        return "/" + ip.toLatin1() + "/" + s->getUri();
    }

    void handleGET(CoapPDU *request, CoapPDU *response, QByteArray *payload);
    void handlePOST(CoapPDU *request, CoapPDU *response){ Q_UNUSED(request); Q_UNUSED(response); qDebug() << "Implement handlePOST";}
    void handlePUT(CoapPDU *request, CoapPDU *response, QHostAddress addr, quint16 port){ Q_UNUSED(request); Q_UNUSED(response); Q_UNUSED(addr); Q_UNUSED(port); qDebug() << "Implement handlePUT";}
    void handleDELETE(CoapPDU *request, CoapPDU *response){ Q_UNUSED(request); Q_UNUSED(response); qDebug() << "Implement handleDELETE";}
};

class notify : public suinterface {
    Q_OBJECT
public:
    notify(suapp* parent, QHostAddress addr, quint16 port) : suinterface(addr, port){
        this->parent = parent;
        const char* uristring = "su/nodeinfo";
        CoapPDU *pdu = new CoapPDU();
        pdu->setURI(const_cast<char*>(uristring), strlen(uristring));
        pdu->addURIQuery(const_cast<char*>("obs"));

        token = put_request(pdu, 100);
    }

protected:
    QVariant parseAppOctetFormat(QByteArray token, QByteArray payload, CoapPDU::Code code) {
        Q_UNUSED(payload); Q_UNUSED(token); Q_UNUSED(code);
        delete this;
        return 0;
    }

    void noResponse() {
        if(this->token == token){
            parent->observerLost(getAddress(), getPort());
        }
        delete this;
    }

private:
    QByteArray token;
    suapp* parent;
};

#endif // SUAPP_H
