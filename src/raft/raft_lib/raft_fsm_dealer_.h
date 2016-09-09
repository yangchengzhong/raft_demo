#ifndef __RAFT_FSM_DEALER_H__
#define __RAFT_FSM_DEALER_H__

#include "raft_context.h"

int raft_fsm_dealer_vote_req_for_term(raft_context_t* r_ctx
        , void *msg_data
        , int msg_data_size
        , bool *need_change_state
        , int *next_state);

int raft_fsm_dealer_on_entry_redirect(raft_context_t* r_ctx
        , void *msg_data
        , int msg_data_size
        , bool *need_change_state
        , int *next_state);

int raft_fsm_dealer_on_recv_entry(raft_context_t* r_ctx
        , void *msg_data
        , int msg_data_size
        , bool *need_change_state
        , int *next_state);

#endif

