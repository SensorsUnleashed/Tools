#include "suapp.h"
#include "log.h"

suapp::suapp()
{
    QSqlDatabase Database;
#if USE_SQLITE
    Database = QSqlDatabase::addDatabase("QSQLITE");
    Database.setDatabaseName("Measurements.sqlite");
#else
    Database = QSqlDatabase::addDatabase("QMYSQL");
    Database.setHostName("192.168.100.100");
    Database.setDatabaseName("Measurements");
    Database.setUserName("loguser");
    Database.setPassword("Martin1977");

#endif
    if (!Database.open())
    {
        qDebug() << Database.lastError().text();
    }

    //Insert the starttime into the table fetch the next logid from the table
    QSqlQuery qry;

#if USE_SQLITE
    QString querystring = "CREATE TABLE IF NOT EXISTS starttime("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,"
                  "Timestamp INTEGER);";
#else
    QString querystring = "CREATE TABLE IF NOT EXISTS `Measurements`.`starttime` ("
      "`id` INT UNSIGNED NULL AUTO_INCREMENT, "
      "`Timestamp` BIGINT NULL, PRIMARY KEY (`id`));";

#endif
    qry.prepare(querystring);

    if( !qry.exec() ){
        qDebug() << qry.lastError();
        return;
    }

    qint64 epoch = QDateTime::currentMSecsSinceEpoch();
    querystring = "INSERT INTO starttime (Timestamp) VALUES(" + QString::number(epoch) + ");";
    qry.prepare(querystring);
    if( !qry.exec() ){
        qDebug() << qry.lastError();
        return;
    }

    querystring = "SELECT id FROM starttime ORDER BY id DESC LIMIT 1;";
    qry.prepare(querystring);
    if( !qry.exec() ){
        qDebug() << qry.lastError();
        return;
    }
    qry.first();
    int logid = qry.value("id").toInt();


    node* n1 = new node(QHostAddress("fd00::212:4b00:3d0:a448"));
    pulsecounter* s1 = new pulsecounter(n1, "su/pulsecounter", QVariantMap());
    sensorlog* l1 = new sensorlog(s1, "pulsecounter", "change", logid);

    node* n2 = new node(QHostAddress("fd00::212:4b00:5af:82b7"));
    //defaultdevice* s2 = new defaultdevice(n2, "su/powerrelay", QVariantMap());
    sensor* s2 = new sensor(n2, "su/powerrelay", QVariantMap());
    sensorlog* l2 = new sensorlog(s2, "powerrelay", "change", logid);

    senserlogmap.insert(s1, l1);
    senserlogmap.insert(s2, l2);

    connect(s1, SIGNAL(rangeMinValueReceived(QVariant)), this, SLOT(rangeMinValueReceived(QVariant)));
    connect(s2, SIGNAL(rangeMinValueReceived(QVariant)), this, SLOT(rangeMinValueReceived(QVariant)));

    s1->requestRangeMin();
    s2->requestRangeMin();
}

void suapp::rangeMinValueReceived(QVariant result){
    (void)result;
    sensor* s = static_cast<sensor*>(sender());

    disconnect(s, SIGNAL(rangeMinValueReceived(QVariant)), this, SLOT(rangeMinValueReceived(QVariant)));
    connect(s, SIGNAL(rangeMaxValueReceived(QVariant)), this, SLOT(rangeMaxValueReceived(QVariant)));
    s->requestRangeMax();
}
void suapp::rangeMaxValueReceived(QVariant result){
    (void)result;
    sensor* s = static_cast<sensor*>(sender());

    disconnect(s, SIGNAL(rangeMaxValueReceived(QVariant)), this, SLOT(rangeMaxValueReceived(QVariant)));
    connect(s, SIGNAL(eventSetupRdy()), this, SLOT(eventSetupRdy()));
    s->req_eventSetup();
}

void suapp::eventSetupRdy(){
    sensor* s = static_cast<sensor*>(sender());
    sensorlog* sl = senserlogmap[s];
    s->requestObserve(sl->getEvent());
    disconnect(s, SIGNAL(eventSetupRdy()), this, SLOT(eventSetupRdy()));
}
