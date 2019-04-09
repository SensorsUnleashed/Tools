#ifndef SUAPP_H
#define SUAPP_H
#include <QObject>
#include <QHash>
#include <node.h>
#include "rdserver.h"

class suapp : public QObject
{
    Q_OBJECT
public:
    suapp();
    virtual ~suapp();
private:
    rdServer* rd;

public slots:
};

#endif // SUAPP_H
