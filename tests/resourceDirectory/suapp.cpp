#include "suapp.h"
#include <QHostAddress>
#include <QUrl>

suapp::suapp()
{
    configuration = new config();

    list = new QVector<obsdevice*>();
    configuration->getObserversList(list);

#if 1
    node* n1 = new node(QHostAddress("fd00::212:4b00:3d0:a448"));   observersLost++;
    node* n2 = new node(QHostAddress("fd00::212:4b00:5af:82b7"));   observersLost++;

    //    sensor* s1 = new sensor("fd00::212:4b00:3d0:a448", "su/pulsecounter");
    //    su_resource* r = new su_resource(s1);
    //    addResource(r);

    //    sensor* s2 = new sensor("fd00::212:4b00:5af:82b7", "dev/test2");
    //    su_resource* r2 = new su_resource(s2);
    //    addResource(r2);

    //    sensor* s3 = new sensor("fd00::212:4b00:5af:82b7", "dev/test3");
    //    su_resource* r3 = new su_resource(s3);
    //    addResource(r3);

    //    sensor* s4 = new sensor("fd00::212:4b00:5af:82b7", "dev/test4");
    //    su_resource* r4 = new su_resource(s4);
    //    addResource(r4);

    n1->requestLinks();
    n2->requestLinks();

    connect(n1, SIGNAL(sensorCreated(sensor*)), this, SLOT(sensorCreated(sensor*)));
    connect(n2, SIGNAL(sensorCreated(sensor*)), this, SLOT(sensorCreated(sensor*)));

    connect(n1, SIGNAL(linkParsingDone()), this, SLOT(linksParsed()));
    connect(n2, SIGNAL(linkParsingDone()), this, SLOT(linksParsed()));

#endif

}

suapp::~suapp(){

}

void suapp::notifyLost(){
    if(--observersLost > 0){
        return;
    }

    int done;
    do{
        done = 0;
        if(list->count()){
            new notify(this, list->first()->addr, list->first()->port);
            delete list->first();
            list->removeFirst();
            done = 1;
        }
    }while(done);
}

void suapp::linksParsed(){
    if(observersLost){
        notifyLost();
    }
}

void suapp::sensorCreated(sensor* s){
    su_resource* r = new su_resource(s);
    addResource(r);

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
    handleObservers(r);
}

void suapp::observerLost(QHostAddress addr, quint16 port){
    configuration->removeObserver(addr, port);
}

void suapp::observerAdded(QHostAddress addr, quint16 port){
    configuration->addObserver(addr, port);
}

void su_resource::handleGET(CoapPDU *request, CoapPDU *response, QByteArray* payload)
{
    char dst[200] = {0};
    int outLen;

    enum CoapPDU::ContentFormat ct = CoapPDU::COAP_CONTENT_FORMAT_TEXT_PLAIN;

    if(request->getURI(dst, 200, &outLen) != 0){
        response->setCode(CoapPDU::COAP_BAD_REQUEST);
        ct = CoapPDU::COAP_CONTENT_FORMAT_TEXT_PLAIN;
        payload->append("This is wrong");
    }
    else{
        QUrl url(dst);
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


