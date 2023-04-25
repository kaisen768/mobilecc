#ifndef _MOBILECC_TOOCHAIN_H_
#define _MOBILECC_TOOCHAIN_H_

#include "ATChannel/atchannel.h"

static int mobilecc_convert_signal(ec20_signal_t signal)
{
    int dBm = 0;
    uint8_t tmp = signal.rssi;

    switch (tmp) {
    case 0:
        dBm = -113;
        break;
    case 1:
        dBm = -111;
        break;
    case 2 ... 30:
        dBm = -(109 - ((tmp-2)*2));
        break;
    case 31:
        dBm = -51;
        break;
    default:
        dBm = 0;
        break;
    }

    return dBm;
}

#endif
