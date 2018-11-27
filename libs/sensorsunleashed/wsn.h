#ifndef WSN_H
#define WSN_H

#include "sensorsunleashed_global.h"
#include <QObject>
#include <QDebug>
#include <QElapsedTimer>
#include <QHostAddress>
#include <QTimer>
#include "cantcoap/cantcoap.h"

#include <QUdpSocket>
#include <QObject>
#include <QVector>

#include "su_message.h"
class wsn;

struct observer{
    QHostAddress id;
    wsn* ref;
};

//struct msgid_s{
//    uint16_t number;
//    uint16_t req;
//};
//typedef struct msgid_s msgid;

struct coapMessageStore_{
    uint16_t token;  //The first messageid, used for finding the right message reply from the gui
    msgid tokenref;
    CoapPDU* lastPDU;    //The inital message send to the node
    QByteArray rx_payload;  //The payload. Will be assembled as the right messages rolls in
    QByteArray tx_payload;
    uint32_t tx_next_index; //What should be send next
    uint32_t num;           //Active number (Not yet acknowledged)
    QElapsedTimer txtime;
    uint8_t keep;
    uint8_t retranscount;
};

class SENSORSUNLEASHEDSHARED_EXPORT socket : public QObject
{
    Q_OBJECT
public:

    explicit socket(QObject *parent = nullptr);
    ~socket();

    static socket* getInstance();
    void observe(wsn *ref, QHostAddress id);
    void observe_stop(wsn* ref);

    void send(QHostAddress addr, uint8_t* pduptr, int len);

private:
    static bool instanceFlag;
    static socket *conn;

    QUdpSocket* udpSocket;
    QVector<struct observer> observerlist;

signals:
    void messageReceived(QHostAddress sender, QByteArray* message);

private slots:
    void readPendingDatagrams();

public slots:
};

class SENSORSUNLEASHEDSHARED_EXPORT wsn : public QObject
{
    Q_OBJECT
public:
    /*explicit*/ wsn(QHostAddress addr);

    void send(CoapPDU *pdu, msgid cmdref, QByteArray payload=nullptr, quint8 allow_retry=0);
    void send_RST(CoapPDU *recvPDU);
    void send_ACK(CoapPDU *recvPDU);

    Q_INVOKABLE void stopListening();
    Q_INVOKABLE void retry(quint16 token);
    Q_INVOKABLE void abort(quint16 token);
    void parseData(QByteArray datagram);
    QVariant parseMessage(coapMessageStore_* message, CoapPDU::Code code);

    //virtual void nodeNotResponding(msgid token){ Q_UNUSED(token); qDebug() << "Implement this";}
    virtual void nodeResponding(msgid token){ Q_UNUSED(token); qDebug() << "Implement this";}
    virtual QVariant parseTextPlainFormat(msgid token, QByteArray payload){ qDebug() << "wsn::parseTextPlainFormat " << payload << " token=" << token.number; return QVariant(0);}
    virtual QVariant parseAppLinkFormat(msgid token, QByteArray payload) { Q_UNUSED(payload); Q_UNUSED(token); qDebug() << "wsn::parseAppLinkFormat Implement this"; return QVariant(0);}
    virtual QVariant parseAppXmlFormat(msgid token, QByteArray payload) { Q_UNUSED(payload); Q_UNUSED(token); qDebug() << "wsn::parseAppXmlFormat Implement this"; return QVariant(0);}
    virtual QVariant parseAppOctetFormat(msgid token, QByteArray payload, CoapPDU::Code code) { Q_UNUSED(payload); Q_UNUSED(token); Q_UNUSED(code); qDebug() << "wsn::parseAppOctetFormat Implement this"; return QVariant(0);}
    virtual QVariant parseAppExiFormat(msgid token, QByteArray payload) { Q_UNUSED(payload); Q_UNUSED(token); qDebug() << "wsn::parseAppExiFormat Implement this"; return QVariant(0);}
    virtual QVariant parseAppJSonFormat(msgid token, QByteArray payload) { Q_UNUSED(payload); Q_UNUSED(token); qDebug() << "wsn::parseAppJSonFormat Implement this"; return QVariant(0);}
    virtual void handleReturnCode(msgid token, CoapPDU::Code code) { Q_UNUSED(token); Q_UNUSED(code); qDebug() << "wsn::handleReturnCode Implement this"; }
    //virtual void tx_progress(uint32_t num, int32_t totallen) {qDebug() << num << "/" << totallen;}

    void disableTokenRemoval(uint16_t token);
    void enableTokenRemoval(uint16_t token);

    void removePDU(uint16_t token);
    int calc_block_option(uint8_t more, uint32_t num, uint32_t msgsize, uint8_t* blockval, uint16_t* len);
    int parseBlockOption(CoapPDU::CoapOption* blockoption, uint8_t* more, uint32_t* num, uint8_t* SZX);
    CoapPDU::CoapOption* coap_check_option(CoapPDU *pdu, enum CoapPDU::Option opt);
    struct coapMessageStore_* findPDU(CoapPDU* pdu);

private:
    QHostAddress addr;

    uint32_t prefMsgSize;
    uint32_t ackTimeout;
    uint8_t retransmissions;
    QTimer* acktimer;

    QVector<struct coapMessageStore_*> activePDUs;
signals:
    void timeoutinfo(QVariant retransnumber, QVariant maxretries);
    void tx_progress(quint32 byteindex, qint32 totallen);
    void tx_timeout(quint16 token);
public slots:

private slots:
    void timeout();
};

#endif // WSN_H
