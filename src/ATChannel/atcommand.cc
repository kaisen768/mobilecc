#include "atchannel.h"

int atchannel::command_indentification()
{
    std::string command;

    ec20_indentification_encode(command);

    return sendto(command);
}

int atchannel::command_signal()
{
    std::string command;

    ec20_signal_encode(command);

    return sendto(command);
}

int atchannel::command_sim_insertion_status()
{
    std::string command;

    ec20_sim_insertion_status_encode(command);

    return sendto(command);
}

int atchannel::command_cpin_required()
{
    std::string command;

    ec20_cpin_required_encode(command);

    return sendto(command);
}

int atchannel::command_phone_functional_set(int func)
{
    std::string command;

    switch (func) {
    case EC20_PHONE_FUNCTION_MIN:
    case EC20_PHONE_FUNCTION_FULL:
    case EC20_PHONE_FUNCTION_DISABLE:
        break;
    
    default:
        return -1;
    }

    ec20_phone_functional_encode(command, EC20_CMD_MOTION_SETUP, func);

    return sendto(command);
}

int atchannel::command_phone_functional_get()
{
    std::string command;

    ec20_phone_functional_encode(command, EC20_CMD_MOTION_INQUIRE);

    return sendto(command);
}

int atchannel::command_ps_attachment_set(int state)
{
    std::string command;

    switch (state) {
    case EC20_PS_ATTACHMENT_DETACHED:
    case EC20_PS_ATTACHMENT_ATTACHED:
        break;
    
    default:
        return -1;
    }

    ec20_ps_attachment_encode(command, EC20_CMD_MOTION_SETUP, state);

    return sendto(command);
}

int atchannel::command_ps_attachment_get()
{
    std::string command;

    ec20_ps_attachment_encode(command, EC20_CMD_MOTION_INQUIRE);

    return sendto(command);
}

int atchannel::command_network_status()
{
    std::string command;

    ec20_network_status_encode(command);

    return sendto(command);
}
