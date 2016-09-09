#include "raft_fsm_dealer_check.h"

int raft_fsm_dealer_check_has_vote(raft_context_t* r_ctx, const raft_fsm_cb_vote_req_t* vote_req)
{
    assert_retval(NULL != r_ctx && NULL != vote_req, ERR_INVALID_ARG);

    if(raft_node_flag_contains(&r_ctx->self_node, E_RAFT_NODE_HAS_VOTE_OTHER)) return ERR_RAFT_ALREADY_VOTE;

    return 0;
}

int raft_fsm_dealer_check_peerterm_gt(raft_context_t* r_ctx, u64 peer_term)
{
    assert_retval(NULL != r_ctx, false);

    return raft_ctx_get_term(r_ctx) < peer_term ? 0 : ERR_RAFT_TERM_INVALID;
}

int raft_fsm_dealer_check_peerterm_ge(raft_context_t* r_ctx, u64 peer_term)
{
    assert_retval(NULL != r_ctx, ERR_INVALID_ARG);

    return raft_ctx_get_term(r_ctx) <= peer_term ? 0 : ERR_RAFT_TERM_INVALID;
}

int raft_fsm_dealer_check_peerterm_le(raft_context_t* r_ctx, u64 peer_term)
{
    assert_retval(NULL != r_ctx, ERR_INVALID_ARG);

    return raft_ctx_get_term(r_ctx) >= peer_term ? 0 : ERR_RAFT_TERM_INVALID;
}

int raft_fsm_dealer_check_can_leader_append_entry(raft_context_t* r_ctx)
{
    return 0;
}

int raft_fsm_dealer_check_prev_log_exist(raft_context_t* r_ctx, u64 prev_log_idx)
{
    assert_retval(NULL != r_ctx, ERR_INVALID_ARG);

    return NULL != raft_ctx_get_entry(r_ctx, prev_log_idx) ? 0 : ERR_RAFT_PREV_LOG_NOTFOUND;
}

int raft_fsm_dealer_check_entry_rsp(raft_context_t* r_ctx, raft_peer_t* peer, u64 peer_term, u64 peer_match_index)
{
    assert_retval(NULL != r_ctx, ERR_INVALID_ARG);

    if(peer_term < raft_ctx_get_term(r_ctx)) return ERR_RAFT_TERM_INVALID;   

    u64 cur_match_index = raft_peer_get_match_index(peer);
    if(INVALID64 == peer_match_index || INVALID64 == cur_match_index) return 0;

    return peer_match_index >= cur_match_index ? 0 : ERR_RAFT_MATCH_INDEX_INVALID;
}

int raft_fsm_dealer_check_entry_in_date(raft_context_t* r_ctx, u64 tail_log_term, u64 tail_log_idx)
{
    assert_retval(NULL != r_ctx, ERR_INVALID_ARG);

    u64 tail_entry_idx = raft_ctx_get_entry_tail_idx(r_ctx);
    if(INVALID64 == tail_entry_idx) return 0;

    if(INVALID64 == tail_log_idx) return ERR_RAFT_INDEX_INVALID;

    raft_entry_t* tail_entry = raft_ctx_get_entry(r_ctx, tail_entry_idx);
    assert_retval(NULL != tail_entry, ERR_INVALID_ARG);

    if(tail_entry->term < tail_log_term) return 0;

    if(tail_entry->term == tail_log_term && tail_log_idx >= tail_entry_idx) return 0;

    return ERR_RAFT_INDEX_INVALID;
}
