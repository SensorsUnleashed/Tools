#ifndef SUAPP_H
#define SUAPP_H
#include <QObject>
#include <QHash>
#include <node.h>
#include "log.h"

class suapp : public QObject
{
    Q_OBJECT
public:
    suapp();

private:
    QHash<sensor*, sensorlog*> senserlogmap;
public slots:
    void eventSetupRdy();
    void rangeMaxValueReceived(QVariant result);
    void rangeMinValueReceived(QVariant result);
};

#endif // SUAPP_H
