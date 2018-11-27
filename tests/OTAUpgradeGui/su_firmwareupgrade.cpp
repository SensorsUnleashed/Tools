#include "su_firmwareupgrade.h"

#include <QFile>
#include <QUrl>

#include <su_message.h>
su_firmwareupgrade::su_firmwareupgrade(QHostAddress nodeaddr) : node(nodeaddr)
{
    su_message* msg = new simple_PUT_msg(uri, su_message::req_swreset);
    qDebug() << "ahaha";
}

