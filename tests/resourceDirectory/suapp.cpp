#include "suapp.h"
#include <QHostAddress>

suapp::suapp()
{

    rd = new rdServer;
    node* n1 = new node(QHostAddress("fd00::212:4b00:3d0:a448"));
    node* n2 = new node(QHostAddress("fd00::212:4b00:5af:82b7"));

    n1->requestLinks();

    rd->addNode(n1);
    rd->addNode(n2);
}

suapp::~suapp(){

}

