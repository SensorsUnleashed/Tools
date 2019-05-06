#include "config.h"
#include <QtSql>
#include "credentials.h"

//ssh -L 3307:fileshare.mnissen.dk:3306 -N araldit@fileshare.mnissen.dk
#define MYSQLTUNNEL 0
config::config()
{   
    QSqlDatabase Database;
    Database = QSqlDatabase::addDatabase("QMYSQL");
#if MYSQLTUNNEL
    Database.setHostName("127.0.0.1");
    Database.setPort(3307);
#else
    Database.setHostName(MYSQL_SERVER_IP);
#endif
    Database.setDatabaseName("resourcesmanager");
    Database.setUserName(MYSQL_USER);
    Database.setPassword(MYSQL_PASS);


    if (Database.open())
    {
        opened = true;
        qDebug() << "Database opened";
    }
    else{
        qDebug() << Database.lastError().text();
        return;
    }
}

void config::removeObserver(QHostAddress addr, quint16 port){
    QString querystring("DELETE FROM observers WHERE addr = '" + addr.toString() + "' AND port = " + QString::number(port) + ";");

    QSqlQuery qry;

    qry.prepare(querystring);

    if( !qry.exec() )
        qDebug() << qry.lastError();
}

void config::addObserver(QHostAddress addr, quint16 port){
    QString querystring("INSERT INTO observers (addr, port) SELECT '" + addr.toString() + "'," + QString::number(port) + " FROM DUAL WHERE NOT EXISTS( SELECT 1 FROM observers WHERE addr = '" + addr.toString() + "' AND port = " + QString::number(port) + ") LIMIT 1;");

    QSqlQuery qry;

    qry.prepare(querystring);

    if( !qry.exec() )
        qDebug() << qry.lastError();
}

void config::getObserversList(QVector<obsdevice*>* list){
    QString querystring("SELECT * FROM observers;");

    QSqlQuery qry;

    qry.prepare(querystring);

    if( !qry.exec() )
        qDebug() << qry.lastError();

    while (qry.next()) {
        obsdevice* d = new obsdevice();
        d->addr = QHostAddress(qry.value(0).toString());
        d->port = static_cast<quint16>(qry.value(1).toInt());
        list->append(d);
    }
}


