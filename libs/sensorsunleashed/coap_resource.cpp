#include "coap_resource.h"
#include <QStringList>
coap_resource::coap_resource(QString resourceDescription)
{

    QStringList slist = resourceDescription.split(';', QString::SkipEmptyParts);
    //QString uri;
    //QVariantMap attributes;

    for(int j=0; j<slist.count(); j++){
        if(j==0){   //Uri
            //For now index 0 is always the uri
            QString str = slist.at(0);
            str.remove(QRegExp("[<>]"));
            if( str.at(0) == '/' ) str.remove( 0, 1 );    //Remove leading "/"

            uri = QByteArray(str.toLatin1().data());
        }
        else{   //Attributes as key value pairs
            QStringList keyval = slist.at(j).split("=");
            if(keyval.size() == 2){
                attributes[keyval[0]] = keyval[1];
            }
        }
    }
}

//Dummy
coap_resource::coap_resource(){
}


