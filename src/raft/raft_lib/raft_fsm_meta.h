#ifndef __RAFT_FSM_META_H__
#define __RAFT_FSM_META_H__

#include "state_machine_def.h"

state_machine_def_t* raft_fsm_get_meta();

int raft_fsm_meta_init();

#endif

