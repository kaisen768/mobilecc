#include <iostream>
#include "mobilecc.h"

int main(int argc, char const *argv[])
{
    fprintf(stderr, "Mobilecc example 2023-04-20 0\n");

#if 0
    #define EC20_AT_TTYUSB_PATH "/dev/ttyUSB2"
    atchannel atchn(EC20_AT_TTYUSB_PATH);

    atchn.start();
#endif

#if 0
    dail netdail;

    process_stream_cb stream_cb = [](ssize_t nread, const char *buf){
        char msg[1025] = {0};
        if (nread <= 0)
            return;
        memmove(msg, buf, nread);
        fprintf(stderr, "%s", msg);
    };

    process_exit_cb exit_cb = [](int exit_status, int term_signal){
       fprintf(stderr, "Process exited with status %d, signal %d\n", exit_status, term_signal);
    };

    netdail.register_stream_callback(stream_cb);
    netdail.register_exit_callback(exit_cb);
    netdail.start();
#endif

    mobilecc mobile;

    mobile.start();

    while (true) {
        sleep(1);
        
        shm_4Gmodule_t *m = mobile.get_4Gmobide_status();

        uint8_t signal = abs(mobilecc_convert_signal(m->signal));

        fprintf(stderr, "mobile status: %d, signal: %d dBm\n", mobile.online(), mobilecc_convert_signal(m->signal));
    }

    return 0;
}


