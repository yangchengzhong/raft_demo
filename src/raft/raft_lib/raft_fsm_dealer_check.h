#ifndef __RAFT_FSM_DEALER_CHECK_H__
#define __RAFT_FSM_DEALER_CHECK_H__

#include "raft_context.h"
#include "raft_fsm_def.h"

int raft_fsm_dealer_check_has_vote(raft_context_t* r_ctx, const raft_fsm_cb_vote_req_t* vote_req);

int raft_fsm_dealer_check_peerterm_gt(raft_context_t* r_ctx, u64 peer_term);

int raft_fsm_dealer_check_peerterm_ge(raft_context_t* r_ctx, u64 peer_term);

int raft_fsm_dealer_check_peerterm_le(raft_context_t* r_ctx, u64 peer_term);

int raft_fsm_dealer_check_can_leader_append_entry(raft_context_t* r_ctx);

int raft_fsm_dealer_check_prev_log_exist(raft_context_t* r_ctx, u64 prev_log_idx);

int raft_fsm_dealer_check_entry_rsp(raft_context_t* r_ctx, raft_peer_t* peer, u64 peer_term, u64 peer_tail_log_idx);

int raft_fsm_dealer_check_entry_in_date(raft_context_t* r_ctx, u64 tail_log_term, u64 tail_log_idx);

#endif

