#include "suapp.h"
#include <QRandomGenerator>
#include <coap_transaction.h>
#include <QCoreApplication>
#include <coap_engine.h>

suapp::suapp(CoapPDU* pdu, bool obs, QHostAddress addr, quint16 port) : suinterface(addr, port)
{
    startpdu = new CoapPDU();
    coap::copyPDU(pdu, startpdu);

    if(obs){
        startObsServer();
    }
    token = request(pdu, 0);
}

suapp::~suapp(){

}

void suapp::startObsServer(){
    qDebug() << "Observing";
    s = new obsserver(this);
}

void suapp::obsNotifyRestart(QHostAddress addr, quint16 port){
    qDebug() << "Restart observers to " << addr << " port " << port;
    coap_engine* conn = coap_engine::getInstance();
    conn->removeZombieTransactions(addr, port);

    CoapPDU* tmp = new CoapPDU();
    coap::copyPDU(startpdu, tmp);
    token = request(tmp, 0);
}

QVariant suapp::parseTextPlainFormat(QByteArray token, QByteArray payload){
    Q_UNUSED(token);
    qDebug() << "RXPayload: " << payload;

    if(!s) QCoreApplication::quit();
    return QVariant(0);
}

QVariant suapp::parseAppLinkFormat(QByteArray token, QByteArray payload) {
    Q_UNUSED(token); qDebug() << "suapp::parseAppLinkFormat Implement this";
    qDebug() << payload;
    if(!s) QCoreApplication::quit();
    return QVariant(0);
}

QVariant suapp::parseAppXmlFormat(QByteArray token, QByteArray payload) {
    Q_UNUSED(token); qDebug() << "suapp::parseAppXmlFormat Implement this";
    qDebug() << payload;
    if(!s) QCoreApplication::quit();
    return QVariant(0);
}

QVariant suapp::parseAppOctetFormat(QByteArray token, QByteArray payload, CoapPDU::Code code) {
    Q_UNUSED(payload); Q_UNUSED(token); Q_UNUSED(code);
    suValue val(payload);
    qDebug() << val.toString();
    if(!s) QCoreApplication::quit();
    return QVariant(0);
}

QVariant suapp::parseAppExiFormat(QByteArray token, QByteArray payload) {
    Q_UNUSED(payload); Q_UNUSED(token); qDebug() << "suapp::parseAppExiFormat Implement this";
    if(!s) QCoreApplication::quit();
    return QVariant(0);
}

QVariant suapp::parseAppJSonFormat(QByteArray token, QByteArray payload) {
    Q_UNUSED(token); qDebug() << "suapp::parseAppJSonFormat Implement this";
    qDebug() << payload;
    if(!s) QCoreApplication::quit();
    return QVariant(0);
}
