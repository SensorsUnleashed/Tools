#ifndef COAP_RESOURCE_H
#define COAP_RESOURCE_H
#include <QString>
#include <QVariantMap>
#include <cantcoap/cantcoap.h>
#include <QtDebug>
#include <QHostAddress>

class coap_resource
{
public:
    coap_resource(QString resourceDescription);
    coap_resource();

    virtual ~coap_resource(){}

    virtual QByteArray getUri(){ return uri; }
    void setUri(QByteArray uri){ this->uri = uri; }
    QVariantMap getAttributes(){ return attributes; }
    int getID(){return id;}
    void setID(int id){ this->id = id;}

    virtual void handleGET(CoapPDU *request, CoapPDU *response, QByteArray* payload){ Q_UNUSED(request); Q_UNUSED(response); Q_UNUSED(payload); qDebug() << "Implement handleGET";}
    virtual void handlePOST(CoapPDU *request, CoapPDU *response){ Q_UNUSED(request); Q_UNUSED(response); qDebug() << "Implement handlePOST";}
    virtual void handlePUT(CoapPDU *request, CoapPDU *response, QHostAddress addr, quint16 port){ Q_UNUSED(request); Q_UNUSED(response); qDebug() << "Implement handlePUT"; Q_UNUSED(addr); Q_UNUSED(port);}
    virtual void handleDELETE(CoapPDU *request, CoapPDU *response){ Q_UNUSED(request); Q_UNUSED(response); qDebug() << "Implement handleDELETE";}

private:
    int id = -1;
    QByteArray uri;
    QHostAddress addr;
    QVariantMap attributes;
};

#endif // COAP_RESOURCE_H
