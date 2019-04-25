#ifndef SUAPP_H
#define SUAPP_H
#include <QObject>
#include <QHash>
#include <node.h>

class suapp : public QObject
{
    Q_OBJECT
public:
    suapp();

private:
    const char* command = "Z000000Z\r\n";
    pulsecounter* s1;

public slots:
    void eventSetupRdy();
    void valueUpdate(quint16 token, QVariant result);
};

#endif // SUAPP_H
