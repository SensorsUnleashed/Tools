#ifndef SUAPP_H
#define SUAPP_H
#include <QObject>
#include <QHostAddress>
#include <node.h>
#include <QUrl>
#include "obsserver.h"
#include <cantcoap/cantcoap.h>

class suapp : public suinterface
{
    Q_OBJECT
public:
    suapp(CoapPDU* pdu, bool obs, QHostAddress addr, quint16 port);
    virtual ~suapp();

    void startObsServer();
    void obsNotifyRestart(QHostAddress addr, quint16 port);

private:
    QVariant parseTextPlainFormat(QByteArray token, QByteArray payload);
    QVariant parseAppLinkFormat(QByteArray token, QByteArray payload);
    QVariant parseAppXmlFormat(QByteArray token, QByteArray payload);
    QVariant parseAppOctetFormat(QByteArray token, QByteArray payload, CoapPDU::Code code);
    QVariant parseAppExiFormat(QByteArray token, QByteArray payload);
    QVariant parseAppJSonFormat(QByteArray token, QByteArray payload);

    obsserver* s = nullptr;
    QByteArray token;
    CoapPDU* startpdu;
public slots:
};

#endif // SUAPP_H
