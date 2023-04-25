#ifndef EC20_COMMAND_PS_ATTACHMENT_H
#define EC20_COMMAND_PS_ATTACHMENT_H

#include "ec20_basic.h"

#define EC20_CMD_PS_ATTACHMENT          "AT+CGATT"
#define EC20_CMD_PS_ATTACHMENT_RESPONSE "+CGATT:"

typedef enum ec20_ps_attachment_state {
    EC20_PS_ATTACHMENT_DETACHED = 0,
    EC20_PS_ATTACHMENT_ATTACHED = 1,
} EC20_PS_ATTACHMENT_STATE_E;

typedef struct ec20_ps_attachment {
    uint8_t state;

    ec20_ps_attachment() {
        state = EC20_PS_ATTACHMENT_DETACHED;
    }
} ec20_ps_attachment_t;

EC20_HELPER void ec20_ps_attachment_decode(std::string data, ec20_ps_attachment_t &attachment)
{
    if (data.empty())
        return;

    ssize_t pos;
    std::string tmp;

    pos = data.find(EC20_CMD_PS_ATTACHMENT_RESPONSE);

    if (pos != std::string::npos) {
        tmp = data.substr(pos + sizeof(EC20_CMD_PS_ATTACHMENT_RESPONSE));
    } else {
        return;
    }

    int state;

    sscanf(tmp.c_str(), "%d", &state);

    attachment.state = state;
}

EC20_HELPER void ec20_ps_attachment_encode(std::string &command, ec20_cmd_motion_t motion, int state = EC20_PS_ATTACHMENT_DETACHED)
{
    switch (motion)
    {
    case EC20_CMD_MOTION_SETUP:
        command = EC20_CMD_PS_ATTACHMENT;
        command.append("=");
        command.append(std::to_string(state));
        command.append("\r\n");
        break;

    case EC20_CMD_MOTION_INQUIRE:
        command = EC20_CMD_PS_ATTACHMENT;
        command.append("?\r\n");
        break;
    }
}

EC20_HELPER void ec20_ps_attachment_debug(const ec20_ps_attachment_t &attachment)
{
    EC20_DEBUG_PRINTF(("\n\n%s\n", EC20_CMD_PS_ATTACHMENT));
    EC20_DEBUG_PRINTF(("**********************************************************\n"));
    EC20_DEBUG_PRINTF(("state :\t\t%d\n", attachment.state));
    EC20_DEBUG_PRINTF(("**********************************************************\n\n"));
}

#endif
