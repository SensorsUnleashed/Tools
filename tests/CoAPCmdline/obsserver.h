#ifndef OBSSERVER_H
#define OBSSERVER_H
#include <coap_server.h>
class suapp;
class nodeinfo : public coap_resource{

public:
    nodeinfo(suapp* parent){
        this->parent = parent;
        setUri("/su/nodeinfo");
    }

    virtual ~nodeinfo(){

    }

private:
    suapp* parent;
    void handlePUT(CoapPDU *request, CoapPDU *response, QHostAddress addr, quint16 port);

};

class obsserver : public coap_server
{
public:
    obsserver(suapp* parent);
};

#endif // OBSSERVER_H
