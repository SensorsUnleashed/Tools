#include "suapp.h"
#include <QHostAddress>

suapp::suapp()
{
    server = new coap_server();

    node* n1 = new node(QHostAddress("fd00::212:4b00:3d0:a448"));
    node* n2 = new node(QHostAddress("fd00::212:4b00:5af:82b7"));

    sensor* s1 = new sensor("fd00::212:4b00:5af:82b7", "dev/test");
    su_resource* r = new su_resource(s1);
    server->addResource(r);

    sensor* s2 = new sensor("fd00::212:4b00:5af:82b7", "dev/test2");
    su_resource* r2 = new su_resource(s2);
    server->addResource(r2);

    sensor* s3 = new sensor("fd00::212:4b00:5af:82b7", "dev/test3");
    su_resource* r3 = new su_resource(s3);
    server->addResource(r3);

    sensor* s4 = new sensor("fd00::212:4b00:5af:82b7", "dev/test4");
    su_resource* r4 = new su_resource(s4);
    server->addResource(r4);

    n1->requestLinks();
    n2->requestLinks();

    connect(n1, SIGNAL(sensorCreated(sensor*)), this, SLOT(sensorCreated(sensor*)));
    connect(n2, SIGNAL(sensorCreated(sensor*)), this, SLOT(sensorCreated(sensor*)));
}

suapp::~suapp(){

}

void suapp::sensorCreated(sensor* s){
    su_resource* r = new su_resource(s);
    server->addResource(r);
    //s->requestValue();
    suValue* v = s->getnLastValue();
    if(v != nullptr){
        s->requestObserve("change");
        connect(v, SIGNAL(valueChanged()), this, SLOT(sensorValueChanged()));
        observees[v] = r;
    }
}

void suapp::sensorValueChanged(){
    suValue* v = static_cast<suValue*>(QObject::sender());

    coap_resource* r = observees[v];
    qDebug() << "sensorValueChanged:: " << r->getUri() << ": " << v->toString();
    server->handleObservers(r);
}

#include <QUrl>
void su_resource::handleGET(CoapPDU *request, CoapPDU *response, QByteArray* payload)
{
    Q_UNUSED(response); qDebug() << "su_resource::handleGET";

    char dst[50] = {0};
    int outLen;

    enum CoapPDU::ContentFormat ct = CoapPDU::COAP_CONTENT_FORMAT_TEXT_PLAIN;

    if(request->getURI(dst, 50, &outLen) != 0){
        response->setCode(CoapPDU::COAP_BAD_REQUEST);
        ct = CoapPDU::COAP_CONTENT_FORMAT_TEXT_PLAIN;
        payload->append("This is wrong");
    }
    else{
        QUrl url(dst);
        qDebug() << url;
        if(url.query().isEmpty()){
            suValue* v =  s->getnLastValue();
            if(v){
                v->get(payload);
                ct = CoapPDU::COAP_CONTENT_FORMAT_APP_OCTET;
            }
        }
        else{
            payload->append("You asked for: " + url.query());
            ct = CoapPDU::COAP_CONTENT_FORMAT_TEXT_PLAIN;
        }

        response->setCode(CoapPDU::COAP_CONTENT);
    }
    response->addOption(CoapPDU::COAP_OPTION_CONTENT_FORMAT,1,reinterpret_cast<uint8_t*>(&ct));
}

