#ifndef EC20_BASIC_H
#define EC20_BASIC_H

#include <iostream>
#include <sstream>
#include <string>

extern "C"{
    #include <stdio.h>
    #include <string.h>
};

#define EC20_HELPER static inline

#define EC20_Debug_message 0

#if EC20_Debug_message
    #define EC20_DEBUG_PRINTF(x)  printf x
#else
    #define EC20_DEBUG_PRINTF(x)
#endif

typedef enum ec20_cmd_motion_e {
    EC20_CMD_MOTION_SETUP = 0,
    EC20_CMD_MOTION_INQUIRE = 1,
} ec20_cmd_motion_t;

#endif
