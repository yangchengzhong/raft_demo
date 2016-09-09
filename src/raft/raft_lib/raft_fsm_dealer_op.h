#ifndef __RAFT_FSM_DEALER_OP_H__
#define __RAFT_FSM_DEALER_OP_H__

#include "raft_fsm_def.h"
#include "raft_context.h"

int raft_fsm_dealer_op_try_remove_by_prev(raft_context_t* r_ctx, int entry_count,  u64 prev_log_idx, u64 prev_log_term);

int raft_fsm_dealer_op_append_entries(raft_context_t* r_ctx
        , u64 prev_log_idx
        , int n_append_entry_arr
        , APPENDENTRY** append_entry_arr);

int raft_fsm_dealder_op_prepare_2_follower(raft_fsm_result_tofollower_t* result_2_follower
        , u64 term
        , u64 candidate_id
        , u64 leader_id
        , bool *need_change_state
        , int *next_state);

void raft_fsm_dealer_op_update_commit_idx(raft_context_t* r_ctx, u64 leader_committed_index);

#endif

