#ifndef EC20_COMMAND_NETWORK_STATUS_H
#define EC20_COMMAND_NETWORK_STATUS_H

#include "ec20_basic.h"

#define EC20_CMD_NETWORK_STATUS             "AT+CREG?"
#define EC20_CMD_NETWORK_STATUS_RESPONSE    "+CREG:"

enum ec20_network_status_stat_e {
    EC20_NETWORK_STAT_NOTREG_NOTSEARCH  = 0,
    EC20_NETWORK_STAT_REGISTED_LOCALNET = 1,
    EC20_NETWORK_STAT_NOTREG_INSEARCH   = 2,
    EC20_NETWORK_STAT_REGDENIED_DENIED  = 3,
    EC20_NETWORK_STAT_UNKNOW            = 4,
    EC20_NETWORK_STAT_REGISTED_ROAMNET  = 5
};

typedef struct ec20_network_status {
    uint8_t n;
    uint8_t stat;

    ec20_network_status() {
        n = 0;
        stat = EC20_NETWORK_STAT_NOTREG_NOTSEARCH;
    }
} ec20_network_status_t;

EC20_HELPER void ec20_network_status_decode(std::string data, ec20_network_status_t &status)
{
    if (data.empty())
        return;

    ssize_t pos;
    std::string tmp;

    pos = data.find(EC20_CMD_NETWORK_STATUS_RESPONSE);

    if (pos != std::string::npos) {
        tmp = data.substr(pos + sizeof(EC20_CMD_NETWORK_STATUS_RESPONSE));
    } else {
        return;
    }

    int n = 0;
    int stat = EC20_NETWORK_STAT_NOTREG_NOTSEARCH;

    sscanf(tmp.c_str(), "%d,%d", &n, &stat);

    status.n = n;
    status.stat = stat;
}

EC20_HELPER void ec20_network_status_encode(std::string &command)
{
    command = EC20_CMD_NETWORK_STATUS;
    command.append("\r\n");
}

EC20_HELPER void ec20_network_status_debug(const ec20_network_status_t &status)
{
    EC20_DEBUG_PRINTF(("\n\n%s\n", EC20_CMD_NETWORK_STATUS));
    EC20_DEBUG_PRINTF(("**********************************************************\n"));
    EC20_DEBUG_PRINTF(("n       :\t\t%u\n", status.n));
    EC20_DEBUG_PRINTF(("stat    :\t\t%u\n", status.stat));
    EC20_DEBUG_PRINTF(("**********************************************************\n\n"));
}

#endif
