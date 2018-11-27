#ifndef SU_MESSAGE_H
#define SU_MESSAGE_H

#include "sensorsunleashed_global.h"

#include <QObject>
#include <QVector>
#include "cantcoap/cantcoap.h"

struct msgid_s{
    quint16 number;
    quint16 req;
};
typedef struct msgid_s msgid;

enum request{
    req_RangeMinValue,
    req_RangeMaxValue,

    req_currentValue,
    req_observe,
    req_aboveEventValue,
    req_belowEventValue,
    req_changeEventAt,

    req_getEventSetup,
    req_saveSetup,
    req_updateEventsetup,

    req_pairingslist,
    req_clearparings,
    req_removepairingitems,
    req_pairsensor,
    /* Used to set a command for an actuator
     * could be togglerelay or other
    */
    req_setCommand,

    /* Used to test a device event handler*/
    req_testevent,

    /* Internal gui events */
    observe_monitor,

    /* System control */
    format_filesystem,
    observe_retry,
    sw_upgrade,
    req_versions,
    req_slotNfoActive,
    req_slotNfoBackup,
    req_activeslot,
    req_coapstatus,
    req_swreset,

    /*border router commnads */
    req_nodeslist,
    req_obs_nodeslist_change,
    req_obs_detect,
};


class SENSORSUNLEASHEDSHARED_EXPORT su_message : public QObject
{
    Q_OBJECT

public:
    enum e_request{
        req_RangeMinValue,
        req_RangeMaxValue,

        req_currentValue,
        req_observe,
        req_aboveEventValue,
        req_belowEventValue,
        req_changeEventAt,

        req_getEventSetup,
        req_saveSetup,
        req_updateEventsetup,

        req_pairingslist,
        req_clearparings,
        req_removepairingitems,
        req_pairsensor,
        /* Used to set a command for an actuator
         * could be togglerelay or other
        */
        req_setCommand,

        /* Used to test a device event handler*/
        req_testevent,

        /* Internal gui events */
        observe_monitor,

        /* System control */
        format_filesystem,
        observe_retry,
        sw_upgrade,
        req_versions,
        req_slotNfoActive,
        req_slotNfoBackup,
        req_activeslot,
        req_coapstatus,
        req_swreset,

        /*border router commnads */
        req_nodeslist,
        req_obs_nodeslist_change,
        req_obs_detect,
        no_of_enums,
    };
    Q_ENUM(e_request)

    su_message(QString path, su_message::e_request command);

//    QByteArray commandstring(e_request cmd){
//        static const auto commands = QHash<e_request, QByteArray>{
//                { req_RangeMinValue, "lastName" },
//                { req_RangeMaxValue, "firstName" }
//            };
//            return roles;
//    }

private:
    char* uristring;
    CoapPDU *pdu;
};

class SENSORSUNLEASHEDSHARED_EXPORT simple_GET_msg : public su_message
{
    Q_OBJECT
public:
    simple_GET_msg(QString path, su_message::e_request command);

private:

};

class SENSORSUNLEASHEDSHARED_EXPORT simple_PUT_msg : public su_message
{
    Q_OBJECT
public:
    simple_PUT_msg(QString path, su_message::e_request command);

private:

};


#endif // SU_MESSAGE_H
