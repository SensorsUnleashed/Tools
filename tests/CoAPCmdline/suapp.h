#ifndef SUAPP_H
#define SUAPP_H
#include <QObject>
#include <QHostAddress>
#include <node.h>
#include <QUrl>

class suapp : public suinterface
{
    Q_OBJECT
public:
    suapp(QUrl *url);
    virtual ~suapp();

private:
    QVariant parseTextPlainFormat(QByteArray token, QByteArray payload);
    QVariant parseAppLinkFormat(QByteArray token, QByteArray payload);
    QVariant parseAppXmlFormat(QByteArray token, QByteArray payload);
    QVariant parseAppOctetFormat(QByteArray token, QByteArray payload, CoapPDU::Code code);
    QVariant parseAppExiFormat(QByteArray token, QByteArray payload);
    QVariant parseAppJSonFormat(QByteArray token, QByteArray payload);

public slots:
};

#endif // SUAPP_H
