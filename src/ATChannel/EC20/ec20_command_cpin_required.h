#ifndef EC20_COMMAND_CPIN_REQUIRED_H
#define EC20_COMMAND_CPIN_REQUIRED_H

#include "ec20_basic.h"

#define EC20_CMD_CPIN_REQUIRED              "AT+CPIN?"
#define EC20_CMD_CPIN_REQUIRED_RESPONSE     "+CPIN:"

#define EC20_CPIN_CODE_READY        "READY"
#define EC20_CPIN_CODE_SIMPIN       "SIM PIN"
#define EC20_CPIN_CODE_SIMPUK       "SIM PUK"
#define EC20_CPIN_CODE_SIMPIN2      "SIM PIN2"
#define EC20_CPIN_CODE_SIMPUK2      "SIM PUK2"
#define EC20_CPIN_CODE_PH_NET_PIN   "PH-NET PIN"
#define EC20_CPIN_CODE_PH_NET_PUK   "PH-NET PUK"

typedef struct ec20_cpin_required {
    std::string code;

    ec20_cpin_required() {
        code.clear();
    }
} ec20_cpin_required_t;

EC20_HELPER void ec20_cpin_required_decode(std::string data, ec20_cpin_required_t &required)
{
    if (data.empty())
        return;

    int i;
    ssize_t pos;
    std::string tmp;
    std::istringstream ss;

    pos = data.find(EC20_CMD_CPIN_REQUIRED_RESPONSE);

    if (pos != std::string::npos) {
        tmp = data.substr(pos + sizeof(EC20_CMD_CPIN_REQUIRED_RESPONSE));
        ss.str(tmp);
    } else {
        return;
    }

    std::string code;
    std::getline(ss, code, '\n');

    required.code = code;
}

EC20_HELPER void ec20_cpin_required_encode(std::string &command)
{
    command = EC20_CMD_CPIN_REQUIRED;
    command.append("\r\n");
}

EC20_HELPER void ec20_cpin_required_debug(const ec20_cpin_required_t &required)
{
    EC20_DEBUG_PRINTF(("\n\n%s\n", EC20_CMD_CPIN_REQUIRED));
    EC20_DEBUG_PRINTF(("**********************************************************\n"));
    EC20_DEBUG_PRINTF(("code :\t\t%s\n", required.code.c_str()));
    EC20_DEBUG_PRINTF(("**********************************************************\n\n"));
}

#endif
