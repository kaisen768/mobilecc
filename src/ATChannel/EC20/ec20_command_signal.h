#ifndef EC20_COMMAND_SIGNAL_H
#define EC20_COMMAND_SIGNAL_H

#include "ec20_basic.h"

#define EC20_CMD_SIGNAL                "AT+CSQ"
#define EC20_CMD_SIGNAL_RESPONSE       "+CSQ:"

typedef struct ec20_signal {
    uint8_t rssi;
    uint8_t ber;

    ec20_signal() {
        rssi = 0;
        ber = 0;
    }
} ec20_signal_t;

EC20_HELPER void ec20_signal_decode(std::string data, ec20_signal_t &signal)
{
    if (data.empty())
        return;

    ssize_t pos;
    std::string tmp;

    pos = data.find(EC20_CMD_SIGNAL_RESPONSE);

    if (pos != std::string::npos) {
        tmp = data.substr(pos);
    } else {
        return;
    }

    int rssi = 0;
    int ber = 0;

    sscanf(tmp.c_str(), "+CSQ: %d,%d", &rssi, &ber);

    signal.rssi = rssi;
    signal.ber = ber;
}

EC20_HELPER void ec20_signal_encode(std::string &command)
{
    command = EC20_CMD_SIGNAL;
    command.append("\r\n");
}

EC20_HELPER void ec20_signal_debug(const ec20_signal_t &signal)
{
    EC20_DEBUG_PRINTF(("\n\n%s\n", EC20_CMD_SIGNAL));
    EC20_DEBUG_PRINTF(("**********************************************************\n"));
    EC20_DEBUG_PRINTF(("rssi:\t\t%u\n", signal.rssi));
    EC20_DEBUG_PRINTF(("ber :\t\t%u\n", signal.ber));
    EC20_DEBUG_PRINTF(("**********************************************************\n\n"));
}

#endif
