#ifndef EC20_COMMAND_PHONE_FUNCTIONAL_H
#define EC20_COMMAND_PHONE_FUNCTIONAL_H

#include "ec20_basic.h"

#define EC20_CMD_PHONE_FUNCTIONAL           "AT+CFUN"
#define EC20_CMD_PHONE_FUNCTIONAL_RESPONSE  "+CFUN:"

typedef enum ec20_phone_function {
    EC20_PHONE_FUNCTION_MIN     = 0,
    EC20_PHONE_FUNCTION_FULL    = 1,
    EC20_PHONE_FUNCTION_DISABLE = 4
} EC20_PHONE_FUNCTION_E;

typedef struct ec20_phone_functional {
    uint8_t fun;

    ec20_phone_functional() {
        fun = 0;
    }
} ec20_phone_functional_t;

EC20_HELPER void ec20_phone_functional_decode(std::string data, ec20_phone_functional_t &functional)
{
    if (data.empty())
        return;

    ssize_t pos;
    std::string tmp;

    pos = data.find(EC20_CMD_PHONE_FUNCTIONAL_RESPONSE);

    if (pos != std::string::npos) {
        tmp = data.substr(pos + sizeof(EC20_CMD_PHONE_FUNCTIONAL_RESPONSE));
    } else {
        return;
    }

    int fun;

    sscanf(tmp.c_str(), "%d", &fun);

    functional.fun = fun;
}

EC20_HELPER void ec20_phone_functional_encode(std::string &command, ec20_cmd_motion_t motion, int fun = 0)
{
    switch (motion)
    {
    case EC20_CMD_MOTION_SETUP:
        command = EC20_CMD_PHONE_FUNCTIONAL;
        command.append("=");
        command.append(std::to_string(fun));
        command.append("\r\n");
        break;

    case EC20_CMD_MOTION_INQUIRE:
        command = EC20_CMD_PHONE_FUNCTIONAL;
        command.append("?\r\n");
        break;
    }
}

EC20_HELPER void ec20_phone_functional_debug(const ec20_phone_functional_t &functional)
{
    EC20_DEBUG_PRINTF(("\n\n%s\n", EC20_CMD_PHONE_FUNCTIONAL));
    EC20_DEBUG_PRINTF(("**********************************************************\n"));
    EC20_DEBUG_PRINTF(("fun :\t\t%d\n", functional.fun));
    EC20_DEBUG_PRINTF(("**********************************************************\n\n"));
}

#endif
