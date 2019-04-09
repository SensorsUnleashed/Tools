#include <QCoreApplication>
#include <QCommandLineParser>
#include <QUrl>


#include "suapp.h"

//./CoAPCmdline -m get coap://[fd00::212:4b00:5af:82b7]/su/powerrelay
//./CoAPCmdline -m put coap://[fd00::212:4b00:5af:82b7]/su/powerrelay?setCommand=2
//./CoAPCmdline -m get coap://[fd00::212:4b00:5af:82b7]/.well-known/core
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

    QCommandLineOption methodOption(QStringList() << "m" << "method", "The request method for action", "get|put|post|delete");
    parser.addOption(methodOption);


    // A boolean option with a single name (-p)
    //QCommandLineOption showProgressOption("p", QCoreApplication::translate("main", "Show progress during copy"));
    //parser.addOption(showProgressOption);

    // A boolean option with multiple names (-f, --force)
    //QCommandLineOption forceOption(QStringList() << "f" << "force",
    //                               QCoreApplication::translate("main", "Overwrite existing files."));
    //parser.addOption(forceOption);

    // An option with a value
    //QCommandLineOption targetDirectoryOption(QStringList() << "t" << "target-directory",
    //                                         QCoreApplication::translate("main", "Copy all source files into <directory>."),
    //                                         QCoreApplication::translate("main", "directory"));
    //parser.addOption(targetDirectoryOption);

    // Process the actual command line arguments given by the user
    parser.process(a);

    const QStringList args = parser.positionalArguments();

    QUrl url(args.at(0));

    if(!url.isValid()){
        qDebug("Invalid URL: %s", qUtf8Printable(url.toString()));
        exit(-1);
    }


    // source is args.at(0), destination is args.at(1)

    //bool showProgress = parser.isSet(showProgressOption);
    //bool force = parser.isSet(forceOption);
    //  QString targetDir = parser.value(targetDirectoryOption);
    // ...

    suapp su(&url);

    CoapPDU* pdu = new CoapPDU();

    char* uristring = url.path().toLatin1().data();
    pdu->setURI(uristring, strlen(uristring));

    if(url.hasQuery()){
        char* query = url.query().toLatin1().data();
        pdu->addURIQuery(query);
    }

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

    enum CoapPDU::ContentFormat ct = CoapPDU::COAP_CONTENT_FORMAT_APP_OCTET;
    pdu->addOption(CoapPDU::COAP_OPTION_CONTENT_FORMAT,1,reinterpret_cast<uint8_t*>(&ct));
    pdu->setMessageID(1);

    su.request(pdu, 0);

    return a.exec();
}