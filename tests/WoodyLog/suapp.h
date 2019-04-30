#ifndef SUAPP_H
#define SUAPP_H
#include <QObject>
#include <QHash>
#include <node.h>
#include <QQmlContext>
#include <QTimer>
#include "woody.h"

class suapp : public QObject
{
    Q_OBJECT
public:
    suapp(QQmlContext *context);

private:
    const char* command = "Z000000Z\r\n";

    pulsecounter* s1;
    QTimer* polltimer;

    woody* w;
public slots:
    void valueUpdate(quint16 token, QVariant result);
    void poll();
};

#endif // SUAPP_H
