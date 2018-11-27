#ifndef SU_FIRMWAREUPGRADE_H
#define SU_FIRMWAREUPGRADE_H

#include <node.h>
#include <QObject>

class su_firmwareupgrade : public node
{
    Q_OBJECT
public:
    su_firmwareupgrade(QHostAddress nodeaddr);
};


#endif // SU_FIRMWAREUPGRADE_H
