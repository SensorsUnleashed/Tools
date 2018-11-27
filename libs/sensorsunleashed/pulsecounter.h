#ifndef PULSECOUNTER_H
#define PULSECOUNTER_H

#include "sensorsunleashed_global.h"
#include <QObject>
#include "node.h"



//class pulsecounter : public sensor
//{
//public:
//    pulsecounter();
//};

class SENSORSUNLEASHEDSHARED_EXPORT pulsecounter : public sensor {
    Q_OBJECT
public:
    pulsecounter(node* parent, QString uri, QVariantMap attributes, sensorstore *p);
    virtual ~pulsecounter(){}
    QVariant getClassType(){ return "PulseCounter.qml"; }

    /*Q_INVOKABLE*/ void startPoll(QVariant interval);

private:
    QTimer* polltimer;

private slots:
    void doPoll();

};

#include "moc_pulsecounter.cpp"

#endif // PULSECOUNTER_H
