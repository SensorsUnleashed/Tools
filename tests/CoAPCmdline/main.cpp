#include <QCoreApplication>
#include <QCommandLineParser>
#include <QUrl>


#include "suapp.h"

//./CoAPCmdline -m get coap://[fd00::212:4b00:5af:82b7]/su/powerrelay
//./CoAPCmdline -m put coap://[fd00::212:4b00:5af:82b7]/su/powerrelay?setCommand=2
//./CoAPCmdline -m get coap://[fd00::212:4b00:5af:82b7]/.well-known/core
//./CoAPCmdline -m get coap://[::1]/.well-known/core

//QTCreator command line
//-m put coap://[fd00::212:4b00:5af:82b7]/su/powerrelay?setCommand=2
//coap://[::1]/fd00::212:4b00:3d0:a448/su/pulsecounter -m get -s
//-m get coap://[::1]/fd00::212:4b00:5af:82b7/dev/test
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Coap commandline tool help");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("uri",
                                 QCoreApplication::translate(
                                     "main", "Destination coap://[::1]:9543 for localhost port 9543"));

    QCommandLineOption methodOption(QStringList() << "m" << "method",
                                    "The request method for action", "get|put|post|delete");
    QCommandLineOption obsOption({"s", "Observe"}, "Subscribe to / observe the resource specified by URI for the given duration in seconds. ");

    parser.addOption(methodOption);
    parser.addOption(obsOption);

    // Process the actual command line arguments given by the user
    parser.process(a);

    const QStringList args = parser.positionalArguments();

    QUrl url(args.at(0));

    if(!url.isValid()){
        qDebug("Invalid URL: %s", qUtf8Printable(url.toString()));
        exit(-1);
    }

    CoapPDU* pdu = new CoapPDU();

    QString u = url.path();

    if(url.hasQuery()){
        u += '?';
        u += url.query();
    }
    char* uristring = u.toLatin1().data();
    pdu->setURI(uristring, strlen(uristring));

    pdu->setType(CoapPDU::COAP_CONFIRMABLE);

    if(parser.value(methodOption).compare("get") == 0){
        pdu->setCode(CoapPDU::COAP_GET);
    }
    else if(parser.value(methodOption).compare("put") == 0){
        pdu->setCode(CoapPDU::COAP_PUT);
    }
    else if(parser.value(methodOption).compare("post") == 0){
        pdu->setCode(CoapPDU::COAP_POST);
    }
    else if(parser.value(methodOption).compare("delete") == 0){
        pdu->setCode(CoapPDU::COAP_POST);
    }
    else{
        qDebug() << "Unknown method";
        exit(-2);
    }

    if(parser.isSet(obsOption)){
        uint8_t obs = 0;
        pdu->addOption(CoapPDU::COAP_OPTION_OBSERVE, 1, &obs);
    }

    enum CoapPDU::ContentFormat ct = CoapPDU::COAP_CONTENT_FORMAT_APP_OCTET;
    pdu->addOption(CoapPDU::COAP_OPTION_CONTENT_FORMAT,1,reinterpret_cast<uint8_t*>(&ct));
    pdu->setMessageID(1);

    suapp su(pdu, parser.isSet(obsOption), QHostAddress(url.host()), static_cast<quint16>((url.port(5683))));

    return a.exec();
}
