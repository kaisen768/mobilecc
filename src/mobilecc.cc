#include "mobilecc.h"

mobilecc::mobilecc()
{
    mobile_setup = MOBILECC_STEUP_IDLE;
    run_state = false;
    _isexist = false;
}

mobilecc::~mobilecc()
{
}

void mobilecc::start()
{
    atchn.start();
    edail.start();

    mobile_setup = MOBILECC_STEUP_ATCHANNEL_CHECK;
    run_state = true;
    _isexist = true;
    mobilethread = std::thread(&mobilecc::setpup_handler, this);
    mobilethread.detach();
}

void mobilecc::stop()
{
    run_state = false;
    while (_isexist) { usleep(1000); }

    atchn.stop();
    edail.stop();
}

bool mobilecc::online()
{
    return edail.online();
}

shm_4Gmodule_t *mobilecc::get_4Gmobide_status()
{
    return atchn.get_shm_4Gmodule();
}

#define MOBILECC_STEUP_JUMPTO(s) do{ mobile_setup = s; } while (0)

void mobilecc::setpup_handler()
{
    while (run_state) {
        switch (mobile_setup)
        {
        /** 
         * NIC ATChannel configuration status detection
         * 
         * @note Check the current state of ATChannel, if the ATChannel module 
         *      is not started, start the module.
         ------------------------------------------------------------------------------- */
        case MOBILECC_STEUP_ATCHANNEL_CHECK:
            if (atchn.isrunning()) {
                if (atchn.isready()) {
                    MOBILECC_STEUP_JUMPTO(MOBILECC_STEUP_DIALPROCS_CHECK);
                }
            }
            break;
        
        /** 
         * Check the status of the dialing process
         * 
         * @note If it is detected that the dialing process is not started, 
         *      the dialing process is started.
         ------------------------------------------------------------------------------- */
        case MOBILECC_STEUP_DIALPROCS_CHECK:
            printf("dail status: %d\n", edail.get_status());

            if (edail.get_status() == DIAL_UNOPENED) {
                edail.start();
            } else if (edail.get_status() == DIAL_PROCESSING) {
                if (edail.online() != true)
                    break;

                MOBILECC_STEUP_JUMPTO(MOBILECC_STEUP_DIALPROCS_HOLDON);
            }
            break;

        /** 
         * Status polling for successful dialing
         * 
         * @mobile_success: The dialing process started successfully
         * @1 Hardware status part status detection
         * @2 Software processing part status detection
         * @3 Log
         *      1) Record online time *
         *      2) Record the dialing exit time and save the first segment of dialing information *
         ------------------------------------------------------------------------------- */
        case MOBILECC_STEUP_DIALPROCS_HOLDON:
            if (edail.online() != true) {
                MOBILECC_STEUP_JUMPTO(MOBILECC_STEUP_DIALPROCS_CHECK);
                break;
            }
            /*
            * Log : dial processor information
            *      @1 online time
            *      ...
            */
            break;

        case MOBILECC_STEUP_IDLE:
        default:
            break;
        }

        sleep(1);
    }

    _isexist = false;
}