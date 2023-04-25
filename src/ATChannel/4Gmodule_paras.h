#ifndef _4GMODULE_PARAS_H_
#define _4GMODULE_PARAS_H_

#include "EC20/common.h"

typedef struct shm_4Gmodule {
    ec20_indentification_t      indentification;
    ec20_signal_t               signal;
    ec20_sim_insertion_status_t sim_insertion_status;
    ec20_cpin_required_t        cpin_required;
    ec20_phone_functional_t     phone_functional;
    ec20_ps_attachment_t        ps_attachment;
    ec20_network_status_t       network_status;
} shm_4Gmodule_t;

shm_4Gmodule_t *get_shm_4Gmodule(void);
void release_shm_4Gmodule(shm_4Gmodule_t *shm);

typedef enum ec20_para_id {
    EC20_ID_INDENTIFICATION,
    EC20_ID_SIM_INSERTION_STATUS,
    EC20_ID_CPIN,
    EC20_ID_PHONE_FUNCTIONAL,
    EC20_ID_PS_ATTACHMENT,
    EC20_ID_NETWORK_STATUS,
} ec20_para_id_t;

#endif
