#include "atchannel.h"

static inline bool certain_command_response(const std::string data, 
                        const char *cmp1, const char *cmp2, const char *cmp3)
{
    bool result;

    if (cmp1) {
        result = (data.find(cmp1) != std::string::npos) ? true : false;
        goto Result;
    }

    if (cmp2) {
        result = (data.find(cmp2) != std::string::npos) ? true : false;
        goto Result;
    }

    if (cmp3) {
        result = (data.find(cmp3) != std::string::npos) ? true : false;
        goto Result;
    }

Result:
    return result;
}

void atchannel::command_handle(const std::string data)
{
    if (nullptr == ec20)
        return;

    if (certain_command_response(data, EC20_CMD_SIGNAL_RESPONSE, nullptr, nullptr)) {
        ec20_signal_decode(data, ec20->signal);
        ec20_signal_debug(ec20->signal);
    }

    else if (certain_command_response(data, EC20_CMD_INDENTIFICATION_RESPONSE, nullptr, nullptr)) {
        ec20_indentification_decode(data, ec20->indentification);
        ec20_indentification_debug(ec20->indentification);
    }

    else if (certain_command_response(data, EC20_CMD_SIM_INSERTION_STATUS_RESPONSE, nullptr, nullptr)) {
        ec20_sim_insertion_status_decode(data, ec20->sim_insertion_status);
        ec20_sim_insertion_status_debug(ec20->sim_insertion_status);
    }

    else if (certain_command_response(data, EC20_CMD_CPIN_REQUIRED_RESPONSE, nullptr, nullptr)) {
        ec20_cpin_required_decode(data, ec20->cpin_required);
        ec20_cpin_required_debug(ec20->cpin_required);
    }

    else if (certain_command_response(data, EC20_CMD_PHONE_FUNCTIONAL_RESPONSE, nullptr, nullptr)) {
        ec20_phone_functional_decode(data, ec20->phone_functional);
        ec20_phone_functional_debug(ec20->phone_functional);
    }

    else if (certain_command_response(data, EC20_CMD_PS_ATTACHMENT_RESPONSE, nullptr, nullptr)) {
        ec20_ps_attachment_decode(data, ec20->ps_attachment);
        ec20_ps_attachment_debug(ec20->ps_attachment);
    }

    else if (certain_command_response(data, EC20_CMD_NETWORK_STATUS_RESPONSE, nullptr, nullptr)) {
        ec20_network_status_decode(data, ec20->network_status);
        ec20_network_status_debug(ec20->network_status);
    }
}
