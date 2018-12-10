#include "log.h"

#include <node.h>
#include <QtSql>

sensorlog::sensorlog(sensor* s, QString tablename, QString event, int logid)
{
    this->s = s;
    this->tablename = tablename;
    this->event = event;
    this->logid = logid;

    if(prepareTables(s->getValueType()) == 0){
        connect(s, SIGNAL(currentValueChanged(quint16, QVariant)), this, SLOT(valueUpdate(quint16, QVariant)));
    }
}

QString sensorlog::getEvent(){
    return event;
}
int sensorlog::prepareTables(int8_t datatype){

    QSqlQuery qry;

    QString dt;
    if(datatype >= CMP_TYPE_UINT8 && datatype <= CMP_TYPE_SINT64){
        dt = "INTEGER";
    }

    QString querystring = "CREATE TABLE IF NOT EXISTS " + tablename +
            "(Timestamp INTEGER, "
            "Value " + dt + ", "
            "Min " + dt + ", "
            "Max " + dt + ", "
             "Logid INTEGER); "
            ;

    // Creating table owner
    qry.prepare(querystring);

    if( !qry.exec() ){
        qDebug() << qry.lastError();
        return 1;
    }
    else
        qDebug() << "Log: Table " + tablename + " created";

    return 0;
}

void sensorlog::valueUpdate(quint16 token, QVariant result){
    (void)token;
    qint64 epoch = QDateTime::currentMSecsSinceEpoch();

    QString value, minstr, maxstr;

    cmp_object_t val = s->getLastValue();
    cmp_object_t max = s->getMaxLimit();
    cmp_object_t min = s->getMinLimit();

    if(cmp_object_is_uinteger(&val)){
        uint64_t v;
        cmp_object_as_uinteger(&val, &v);
        value = QString::number(v);

        cmp_object_as_uinteger(&min, &v);
        minstr = QString::number(v);

        cmp_object_as_uinteger(&max, &v);
        maxstr = QString::number(v);
    }

    QString querystring = "INSERT INTO " + tablename + "(Timestamp,Value,Min,Max,Logid) VALUES (" + QString::number(epoch) +"," + value + ", " + minstr + "," + maxstr + "," + QString::number(logid) + ");";

    QSqlQuery qry;

    qry.prepare(querystring);

    if( !qry.exec() )
        qDebug() << qry.lastError();

    qDebug() << "Log: " << " " << s->getUri() << " : " << result;
}

