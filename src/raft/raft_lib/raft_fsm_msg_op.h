#ifndef __RAFT_FSM_MSG_OP_H__
#define __RAFT_FSM_MSG_OP_H__

#include "raft_context.h"

int raft_fsm_msg_push_election_timeout_msg(raft_context_t* raft_ctx);

int raft_fsm_msg_push_vote_req_msg(raft_context_t* raft_ctx, raft_peer_t* peer, u64 reqer_id, u64 reqer_term, u64 last_entry_term, u64 last_entry_idx);

int raft_fsm_msg_push_vote_rsp_msg(raft_context_t* raft_ctx, raft_peer_t* peer, int ret, u64 target_id, u64 target_term);

int raft_fsm_msg_push_recv_appendentry_msg(raft_context_t* raft_ctx
        , raft_peer_t* peer
        , u64 leader_id
        , u64 leader_term
        , u64 leader_committed_index
        , u64 prev_log_idx
        , u64 prev_log_term
        , int n_append_entry_arr
        , APPENDENTRY* append_entry_arr[]);

int raft_fsm_msg_push_on_entry_msg(raft_context_t* raft_ctx, ds_t* entry_content);

int raft_fsm_msg_push_append_entry_timeout_msg(raft_context_t* raft_ctx);

int raft_fsm_msg_push_recv_appendentry_rsp_msg(raft_context_t* raft_ctx
        , raft_peer_t* peer
        , int ret
        , u64 peer_term
        , u64 peer_tail_log_idx);

#endif

