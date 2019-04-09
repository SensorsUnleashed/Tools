#include "wsn.h"

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

//        CoapPDU *recvPDU = new CoapPDU(reinterpret_cast<uint8_t*>(datagram.data()),datagram.length());
//        if(recvPDU->validate()) {

//            CoapPDU::Code code = recvPDU->getCode();
//            if(code >= CoapPDU::COAP_GET && code <= CoapPDU::COAP_DELETE){
//                /* Request - This message is for the server class */
//                if(serverref != nullptr){
//                    serverref->handleRequest(recvPDU);
//                }
//            }
//            else{
//                /* Response - This message is a response of a request made from one of our sensor/node classes */

//                /* Now pass this message on to the right receiver */
//                for(int i=0; i<observerlist.count(); i++){
//                    if(observerlist.at(i).id == sender)
//                        observerlist.at(i).ref->parseData(recvPDU);
//                }
//            }
//        }
//        else{
//            delete recvPDU;
//        }
    }
}
