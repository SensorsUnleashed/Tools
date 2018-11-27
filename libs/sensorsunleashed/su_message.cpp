#include "su_message.h"
#include <QVariant>

su_message::su_message(QString path, e_request command)
{
    QString cmd = QVariant::fromValue(command).value<QString>();
    uristring = path.toLatin1().data();
    pdu = new CoapPDU();
    pdu->setURI(uristring, static_cast<int>(strlen(uristring)));
    pdu->addURIQuery(cmd.toLatin1().data());
}

//Assemble
//Parse

simple_PUT_msg::simple_PUT_msg(QString path, e_request command) : su_message(path, command){
//    msgid t;
//    t.req = req;
//    t.number = qrand();

//    pdu->setType(CoapPDU::COAP_CONFIRMABLE);
//    pdu->setCode(CoapPDU::COAP_GET);
//    pdu->setToken((uint8_t*)&t.number,2);

//    enum CoapPDU::ContentFormat ct = CoapPDU::COAP_CONTENT_FORMAT_APP_OCTET;
//    pdu->addOption(CoapPDU::COAP_OPTION_CONTENT_FORMAT,1,(uint8_t*)&ct);
//    pdu->setMessageID(t.number);

//    char tmp[30];
//    int len;
//    pdu->getURI(tmp,30, &len);
}

simple_GET_msg::simple_GET_msg(QString path, e_request command) : su_message(path, command){

}
