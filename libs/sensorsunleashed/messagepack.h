#ifndef MESSAGEPACK_H
#define MESSAGEPACK_H

#include "sensorsunleashed_global.h"

#include "cmp.h"
#include <QByteArray>
#include <QVariant>

class SENSORSUNLEASHEDSHARED_EXPORT suValue : public QObject
{
    Q_OBJECT
public:
    suValue(cmp_object_t val);
    suValue(QByteArray buffer);

    void update(cmp_object_t val);

    int get(QByteArray* buffer);
    QString toString();
private:
    uint8_t type;
    cmp_ctx_t cmp;
    cmp_object_t val;

    void init(QByteArray *buffer);

signals:
    void valueChanged();
};

#endif // MESSAGEPACK_H
