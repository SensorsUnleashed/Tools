#include "suapp.h"
#include <QRandomGenerator>
#include <coap_transaction.h>
#include <QCoreApplication>

suapp::suapp(QUrl* url) : suinterface(QHostAddress(url->host()), url->port(5683))
{
    qDebug() << url->scheme();
    qDebug() << url->host();
    qDebug() << url->port();
    qDebug() << url->path();
}

suapp::~suapp(){

}

QVariant suapp::parseTextPlainFormat(QByteArray token, QByteArray payload){
    qDebug() << "suapp::parseTextPlainFormat " << payload << " token=" << token; return QVariant(0);
}

QVariant suapp::parseAppLinkFormat(QByteArray token, QByteArray payload) {
    Q_UNUSED(token); qDebug() << "suapp::parseAppLinkFormat Implement this";
    qDebug() << payload;
    QCoreApplication::quit();
    return QVariant(0);
}

QVariant suapp::parseAppXmlFormat(QByteArray token, QByteArray payload) {
    Q_UNUSED(token); qDebug() << "suapp::parseAppXmlFormat Implement this";
    qDebug() << payload;
    QCoreApplication::quit();
    return QVariant(0);
}

QVariant suapp::parseAppOctetFormat(QByteArray token, QByteArray payload, CoapPDU::Code code) {
    Q_UNUSED(payload); Q_UNUSED(token); Q_UNUSED(code); qDebug() << "suapp::parseAppOctetFormat Implement this";
    QCoreApplication::quit();
    return QVariant(0);
}

QVariant suapp::parseAppExiFormat(QByteArray token, QByteArray payload) {
    Q_UNUSED(payload); Q_UNUSED(token); qDebug() << "suapp::parseAppExiFormat Implement this";
    QCoreApplication::quit();
    return QVariant(0);
}

QVariant suapp::parseAppJSonFormat(QByteArray token, QByteArray payload) {
    Q_UNUSED(token); qDebug() << "suapp::parseAppJSonFormat Implement this";
    qDebug() << payload;
    QCoreApplication::quit();
    return QVariant(0);
}
