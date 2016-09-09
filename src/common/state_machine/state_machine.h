#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

#include "state_machine_def.h"

int state_machine_def_init(state_machine_def_t *def);
int state_machine_msg_dealer(state_machine_def_t *state_machine_def, state_machine_t *sm, int msg, void *msg_data, int msg_data_size);

int state_machine_inst_init(state_machine_def_t *state_machine_def, state_machine_t *sm, int init_state, void *msg_data, int msg_data_size);
int state_machine_inst_get_state(const state_machine_t *sm);

void state_machine_timeout(uv_timer_t* handle);

#endif  


