#include "suapp.h"
#include <QHostAddress>
#include <QUrl>

suapp::suapp(config *configuration)
{
    this->configuration = configuration;

    observerList = new QVector<obsdevice*>();
    configuration->getObserversList(observerList);

    QVector<nodeinfo*> nodelist;
    configuration->getNodes(&nodelist);

    for(int i=0; i<nodelist.count(); i++){
        node* n= new node(nodelist.at(i)->addr, nodelist.at(i)->port);
        n->setID(nodelist.at(i)->nodeid);
        connect(n, SIGNAL(sensorCreated(sensor*)), this, SLOT(sensorCreated(sensor*)));

        QVector<deviceinfo*> devicelist;
        configuration->getDevices(nodelist.at(i)->nodeid, &devicelist);
        if(devicelist.isEmpty()){
            n->requestLinks();
            connect(n, SIGNAL(linkParsingDone()), this, SLOT(linksParsed()));
        }
        else{
            nodeDataPending--;
            for(int j=0; j<devicelist.count(); j++){
                coap_resource* r = new coap_resource();
                r->setUri(devicelist.at(j)->uri);
                r->setID(devicelist.at(j)->deviceid);
                n->addSensor(r);
            }
        }
    }

    if(observerList->count()){
        notifyLost();
    }

}

suapp::~suapp(){

}

void suapp::notifyLost(){
    if(nodeDataPending){
        return;
    }

    int done;
    do{
        done = 0;
        if(observerList->count()){
            new notify(this, observerList->first()->addr, observerList->first()->port);
            delete observerList->first();
            observerList->removeFirst();
            done = 1;
        }
    }while(done);
}

/* Called when we asked a node for its resources */
void suapp::linksParsed(){
    nodeDataPending--;
    notifyLost();
}

/* Called when a node has created a new device */
void suapp::sensorCreated(sensor* s){
    su_resource* r = new su_resource(s);
    addResource(r);

    suValue* v = s->getnLastValue();
    if(v != nullptr){
        s->requestObserve("change");
        connect(v, SIGNAL(valueChanged()), this, SLOT(sensorValueChanged()));
        observees[v] = r;
    }

    if(s->getID() < 0){
        s->setID(configuration->addDevice(s->getParent()->getID(), s->getUri(), s->getType()));
    }
}

/* Called when a device has changed its main value */
void suapp::sensorValueChanged(){
    suValue* v = static_cast<suValue*>(QObject::sender());

    su_resource* r = observees[v];
    sensor* s = r->getSensor();
    qDebug() << "sensorValueChanged:: " << s->getUri() << ": " << v->toString();
    configuration->insertNewData(s->getID(), v->toString());

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


