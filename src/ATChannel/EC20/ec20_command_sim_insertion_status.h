#ifndef EC20_COMMAND_SIM_INSERTION_STATUS_H
#define EC20_COMMAND_SIM_INSERTION_STATUS_H

#include "ec20_basic.h"

#define EC20_CMD_SIM_INSERTION_STATUS                "AT+QSIMSTAT?"
#define EC20_CMD_SIM_INSERTION_STATUS_RESPONSE       "+QSIMSTAT:"

typedef struct ec20_sim_insertion_status {
    uint8_t enable;
    uint8_t insertedstatus;

    ec20_sim_insertion_status() {
        enable = 0;
        insertedstatus = 0;
    }
} ec20_sim_insertion_status_t;

EC20_HELPER void ec20_sim_insertion_status_decode(std::string data, ec20_sim_insertion_status_t &status)
{
    if (data.empty())
        return;

    ssize_t pos;
    std::string tmp;

    pos = data.find(EC20_CMD_SIM_INSERTION_STATUS_RESPONSE);

    if (pos != std::string::npos) {
        tmp = data.substr(pos);
    } else {
        return;
    }

    int enable = 0;
    int insertedstatus = 0;

    sscanf(tmp.c_str(), EC20_CMD_SIM_INSERTION_STATUS_RESPONSE" %d,%d", &enable, &insertedstatus);

    status.enable = enable;
    status.insertedstatus = insertedstatus;
}

EC20_HELPER void ec20_sim_insertion_status_encode(std::string &command)
{
    command = EC20_CMD_SIM_INSERTION_STATUS;
    command.append("\r\n");
}

EC20_HELPER void ec20_sim_insertion_status_debug(const ec20_sim_insertion_status_t &status)
{
    EC20_DEBUG_PRINTF(("\n\n%s\n", EC20_CMD_SIM_INSERTION_STATUS));
    EC20_DEBUG_PRINTF(("**********************************************************\n"));
    EC20_DEBUG_PRINTF(("enable          :\t\t%u\n", status.enable));
    EC20_DEBUG_PRINTF(("insertedstatus  :\t\t%u\n", status.insertedstatus));
    EC20_DEBUG_PRINTF(("**********************************************************\n\n"));
}

#endif
