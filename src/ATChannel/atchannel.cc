#include "atchannel.h"

atchannel::atchannel(std::string _device_name, int _baudrate, int _bitnum, int _stopbits, int _parity) : 
    uartbase(_device_name, _baudrate, _bitnum, _stopbits, _parity)
{
#if MOUDLE_PARAS_USE_SHM
    ec20 = get_shm_4Gmodule();
#else
    ec20 = new shm_4Gmodule_t;
#endif

    run_state = false;
    _isexist = false;
}

atchannel::~atchannel()
{
#if MOUDLE_PARAS_USE_SHM
    release_shm_4Gmodule(ec20);
#else
    delete ec20;
#endif
}

void atchannel::start()
{
#if MOUDLE_PARAS_USE_SHM
    if (nullptr == ec20)
        ec20 = get_shm_4Gmodule();
#else
    if (nullptr == ec20)
        ec20 = new shm_4Gmodule_t;
#endif

    uartbase::start();

    run_state = true;
    _isexist = true;
    setupstep = SETUPSTEP_IDENTIFICATION;
    setupthread = std::thread(&atchannel::setup_handler, this);
    setupthread.detach();
}

void atchannel::stop()
{
    setupstep = SETUPSTEP_IDENTIFICATION;
    run_state = false;

    while (_isexist) { usleep(1000); }

    uartbase::stop();
}

bool atchannel::isrunning()
{
    return run_state;
}

bool atchannel::isready()
{
    return (setupstep == SETUPSTEP_READY);
}

shm_4Gmodule_t *atchannel::get_shm_4Gmodule()
{
    return ec20;
}

int atchannel::sendto(std::string command)
{
    return send(command.c_str(), command.length());
}

void atchannel::onreceive(int nread, const char *buf)
{
    std::string data = buf;

    // fprintf(stderr, "onreceive nread:%d\n", nread);
    // fprintf(stderr, "%s\n", buf);

    command_handle(data);
}

static inline bool judge_4Gmodule_para(shm_4Gmodule_t *ec20, ec20_para_id_t id)
{
    bool judge = false;

    if (!ec20)
        return false;

    switch (id)
    {
    case EC20_ID_INDENTIFICATION:
        judge = (ec20->indentification.manufacturer.find(EC20_INDENTIFICATION_IDS) == std::string::npos) ? false : true;
        break;

    case EC20_ID_SIM_INSERTION_STATUS:
        judge = (ec20->sim_insertion_status.insertedstatus == 1) ? true : false;
        break;

    case EC20_ID_CPIN:
        judge = (ec20->cpin_required.code.find(EC20_CPIN_CODE_READY) == std::string::npos) ? false : true;
        break;

    case EC20_ID_PHONE_FUNCTIONAL:
        judge = (ec20->phone_functional.fun == EC20_PHONE_FUNCTION_FULL) ? true : false;
        break;

    case EC20_ID_PS_ATTACHMENT:
        judge = (ec20->ps_attachment.state == EC20_PS_ATTACHMENT_ATTACHED) ? true : false;
        break;

    case EC20_ID_NETWORK_STATUS:
        if ((ec20->network_status.stat == EC20_NETWORK_STAT_REGISTED_LOCALNET) || \
            (ec20->network_status.stat == EC20_NETWORK_STAT_REGISTED_ROAMNET))
        {
            judge = true;
        } else {
            judge = false;
        }
        break;

    default:
        break;
    }

    return judge;
}

#define SETUPSTEP_JUMPTO(s) do{ setupstep = s; } while (0)

void atchannel::setup_handler()
{
#define TIMER_INTERVAL 500

    unsigned int timecount = 0;
    bool gatt_long_wait = false;
    unsigned int gatt_time_count = 0;

    while (run_state) {
        switch (setupstep)
        {
        case SETUPSTEP_IDENTIFICATION:
            if (judge_4Gmodule_para(ec20, EC20_ID_INDENTIFICATION) == false) {
                command_indentification();
            } else {
                SETUPSTEP_JUMPTO(SETUPSTEP_SIM_CHECK);
            }
            break;
        
        case SETUPSTEP_SIM_CHECK:
            if (judge_4Gmodule_para(ec20, EC20_ID_SIM_INSERTION_STATUS) == false) {
                command_sim_insertion_status();
            } else {
                SETUPSTEP_JUMPTO(SETUPSTEP_CPIN_CHECK);
            }
            break;

        case SETUPSTEP_CPIN_CHECK:
            if (judge_4Gmodule_para(ec20, EC20_ID_CPIN) == false) {
                command_cpin_required();
            } else {
                SETUPSTEP_JUMPTO(SETUPSTEP_G_FUN);
            }
            break;

        case SETUPSTEP_S_FUN_FULL:
            if (judge_4Gmodule_para(ec20, EC20_ID_PHONE_FUNCTIONAL) == false) {
                command_phone_functional_set(EC20_PHONE_FUNCTION_FULL);
                SETUPSTEP_JUMPTO(SETUPSTEP_G_FUN);
            } else {
                SETUPSTEP_JUMPTO(SETUPSTEP_G_GATT);
                gatt_long_wait = false;
            }
            break;

        case SETUPSTEP_S_GATT:
            gatt_long_wait = true;
            gatt_time_count = 0;
            command_ps_attachment_set(EC20_PS_ATTACHMENT_ATTACHED);
            SETUPSTEP_JUMPTO(SETUPSTEP_G_GATT);
            break;

        case SETUPSTEP_G_FUN:
            command_phone_functional_get();
            SETUPSTEP_JUMPTO(SETUPSTEP_S_FUN_FULL);
            break;

        case SETUPSTEP_G_GATT:
            if (judge_4Gmodule_para(ec20, EC20_ID_PS_ATTACHMENT) == false) {
                if (!(timecount%2))
                    command_ps_attachment_get();
            } else {
                SETUPSTEP_JUMPTO(SETUPSTEP_CREG_CHECK);
                break;
            }

            if (gatt_long_wait) {
                gatt_time_count++;
                if (gatt_time_count > 280)
                    SETUPSTEP_JUMPTO(SETUPSTEP_S_GATT);
            } else {
                SETUPSTEP_JUMPTO(SETUPSTEP_S_GATT);
            }
            break;

        case SETUPSTEP_CREG_CHECK:
            if (judge_4Gmodule_para(ec20, EC20_ID_NETWORK_STATUS) == false) {
                command_network_status();
            } else {
                fprintf(stderr, "Network Normal\n");
                SETUPSTEP_JUMPTO(SETUPSTEP_READY);
            }
            break;

        case SETUPSTEP_READY:
            if (!(timecount%2)) {
                command_signal();
            }
            break;

        default:
            break;
        }

        timecount++;

        usleep(500*1000);
    }

    _isexist = false;
}
