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

    virtual QByteArray getUri(){ return uri; }
    void setUri(QByteArray uri){ this->uri = uri; }
    QVariantMap getAttributes(){ return attributes; }


    virtual void handleGET(CoapPDU *request, CoapPDU *response, QByteArray* payload){ Q_UNUSED(request); Q_UNUSED(response); Q_UNUSED(payload); qDebug() << "Implement handleGET";}
    virtual void handlePOST(CoapPDU *request, CoapPDU *response){ Q_UNUSED(request); Q_UNUSED(response); qDebug() << "Implement handlePOST";}
    virtual void handlePUT(CoapPDU *request, CoapPDU *response){ Q_UNUSED(request); Q_UNUSED(response); qDebug() << "Implement handlePUT";}
    virtual void handleDELETE(CoapPDU *request, CoapPDU *response){ Q_UNUSED(request); Q_UNUSED(response); qDebug() << "Implement handleDELETE";}

private:
    QByteArray uri;
    QHostAddress addr;
    QVariantMap attributes;
};

#endif // COAP_RESOURCE_H
