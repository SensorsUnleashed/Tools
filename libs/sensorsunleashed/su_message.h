#ifndef SU_MESSAGE_H
#define SU_MESSAGE_H

#include "sensorsunleashed_global.h"

#include <QObject>
#include <QVector>
#include "cantcoap/cantcoap.h"

namespace su_message {
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
}

#endif // SU_MESSAGE_H
