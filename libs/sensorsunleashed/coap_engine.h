#ifndef COAP_ENGINE_H
#define COAP_ENGINE_H

#include <QUdpSocket>
#include "coap_transaction.h"

class SENSORSUNLEASHEDSHARED_EXPORT socket : public QObject
{
    Q_OBJECT
public:

    explicit socket(QObject *parent = nullptr);
    ~socket();

    void send(QHostAddress addr, uint8_t* pduptr, int len, quint16 port);

    virtual void receive(QHostAddress addr, QByteArray datagram, quint16 port){
        Q_UNUSED(addr); Q_UNUSED(datagram); Q_UNUSED(port);
    }

protected:
    void bindTo(quint16 port);
    void close();
private:
    static bool instanceFlag;
    static socket *conn;
    bool bound;

    QUdpSocket* udpSocket;

signals:
    void messageReceived(QHostAddress sender, QByteArray* message, quint16 port);

private slots:
    void readPendingDatagrams();

public slots:
};

class coap_engine : public socket
{
    Q_OBJECT

public:
    coap_engine();

    static coap_engine* getInstance();

    void addServerInstance(coap_server* ref);
    void rmServerInstance(coap_server* ref);

    void transactionListAdd(coap_transaction* transaction);

    void receive(QHostAddress addr, QByteArray datagram, quint16 port);

private:
    coap_server* serverref = nullptr;

    static bool instanceFlag;
    static coap_engine *coapEngine;

    QVector<coap_transaction*> transactionList;


    //QUdpSocket* udpSocket;
    //QVector<struct observer> observerlist;

public slots:
    void transactionTimeout();

};

#endif // COAP_ENGINE_H
