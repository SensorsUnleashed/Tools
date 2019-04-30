#include "su_firmwareupgrade.h"

#include <QFile>
#include <QUrl>

su_firmwareupgrade::su_firmwareupgrade(QHostAddress nodeaddr) : node(nodeaddr)
{
    request_swreset();

    qDebug() << "ahaha";
}

