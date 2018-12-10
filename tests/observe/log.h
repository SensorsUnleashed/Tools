#ifndef LOG_H
#define LOG_H

#include <QObject>
#include <QVariant>
#include <QVector>
#include <QtSql>
#include "node.h"

class sensorlog : public QObject
{
    Q_OBJECT
public:
    explicit sensorlog(sensor *s, QString tablename, QString event, int logid);

    QString getEvent();
private:

    int prepareTables(int8_t datatype);
    QString event;
    sensor* s;
    quint16 tokenref;
    QString tablename;

    int logid;
signals:

public slots:
    void valueUpdate(quint16 token, QVariant result);

};

#endif // LOG_H
