#include "coap_engine.h"

bool socket::instanceFlag = false;
socket* socket::conn = nullptr;

socket::socket(QObject *parent) : QObject(parent)
{
    udpSocket = new QUdpSocket(this);

    connect(udpSocket, SIGNAL(readyRead()),
            this, SLOT(readPendingDatagrams()));
}

socket::~socket()
{
    instanceFlag = false;
    udpSocket->close();
    delete udpSocket;
}

void socket::bindTo(quint16 port){
    if(udpSocket->bind(QHostAddress::AnyIPv6, port)){
        qDebug() << "Successfully bound to port" << port;
        bound = true;
    }
    else{
        qDebug() << "Unable to bind to port" << port;
        qDebug() << udpSocket->errorString();
    }
}

void socket::close(){
    udpSocket->close();
}

void socket::send(QHostAddress addr, uint8_t* pduptr, int len, quint16 port){
    if(!bound){
        bindTo(0);  //0 means random port
    }
    udpSocket->writeDatagram(reinterpret_cast<char*>(pduptr), len, addr, port);
}

void socket::readPendingDatagrams(){
    while (udpSocket->hasPendingDatagrams()) {
        //Create the bytearray and have the one using it handle it.
        QByteArray datagram;
        datagram.resize(static_cast<int>(udpSocket->pendingDatagramSize()));
        QHostAddress sender;
        quint16 senderPort;

        udpSocket->readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);     

        receive(sender, datagram, senderPort);
    }
}
