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

    QString dt, ts;
    if(datatype >= CMP_TYPE_UINT8 && datatype <= CMP_TYPE_SINT64){
#if USE_SQLITE
        dt = "INTEGER";
        ts = "INTEGER";
#else
        dt = "INT";
        ts = "BIGINT";
#endif
    }

    QString querystring = "CREATE TABLE IF NOT EXISTS " + tablename +
            "(Timestamp " + ts + ", "
            "Value " + dt + ", "
            "Min " + dt + ", "
            "Max " + dt + ", "
             "Logid INTEGER); "
            ;

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

    value = s->getnLastValue()->toString();
    minstr = s->getMinLimit()->toString();
    maxstr = s->getMaxLimit()->toString();

    QString querystring = "INSERT INTO " + tablename + "(Timestamp,Value,Min,Max,Logid) VALUES (" + QString::number(epoch) +"," + value + ", " + minstr + "," + maxstr + "," + QString::number(logid) + ");";

    QSqlQuery qry;

    qry.prepare(querystring);

    if( !qry.exec() )
        qDebug() << qry.lastError();

    qDebug() << "Log: " << " " << s->getUri() << " : " << result;
}

