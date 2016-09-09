#ifndef __RAFT_FSM_LEADER_H__
#define __RAFT_FSM_LEADER_H__

#include <stdbool.h>

int raft_fsm_leader_state_dealer(void *fsm_holder, int msg, void *msg_data, int msg_data_size, bool *need_change_state, int *next_state);

void raft_fsm_leader_leave_action(void *fsm_holder, void *msg_data, int msg_data_size);

void raft_fsm_leader_enter_action(void *fsm_holder, void *msg_data, int msg_data_size);

#endif

