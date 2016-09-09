#include "raft_recv_.h"

#include "raft_context.h"
#include "r_ctx.h"
#include "raft_fsm_msg_op.h"

void r_recv_vote_req(raft_peer_t* peer, const RAFTPKG* pkg)
{
    assert_retnone(NULL != peer && NULL != pkg);

    LOGD("r_recv_vote_req");

    PKGVOTESELFREQ *vote_req = pkg->vote_self_req;
    raft_context_t* r_ctx = r_ctx_get_raft();
    assert_retnone(NULL != vote_req && NULL != r_ctx);

    raft_fsm_msg_push_vote_req_msg(r_ctx
            , peer
            , vote_req->reqer_id
            , vote_req->reqer_term
            , vote_req->last_entry_term
            , vote_req->last_entry_idx);

    return ;
}

void r_recv_vote_rsp(raft_peer_t* peer, const RAFTPKG* pkg)
{
    assert_retnone(NULL != peer && NULL != pkg);

    LOGD("r_recv_vote_rsp");

    PKGVOTESELFRSP *vote_rsp = pkg->vote_self_rsp;
    raft_context_t* r_ctx = r_ctx_get_raft();
    assert_retnone(NULL != vote_rsp && NULL != r_ctx);

    raft_fsm_msg_push_vote_rsp_msg(r_ctx, peer, vote_rsp->ret, vote_rsp->target_id, vote_rsp->target_term);

    return ;
}

void r_recv_append_entries_req(raft_peer_t* peer, const RAFTPKG* pkg)
{
    assert_retnone(NULL != peer && NULL != pkg);

    LOGD("r_recv_append_entries_req");

    PKGAPPENDENTRIESREQ *append_entries_req = pkg->append_entries_req;
    raft_context_t* r_ctx = r_ctx_get_raft();
    assert_retnone(NULL != append_entries_req && NULL != r_ctx);

    raft_fsm_msg_push_recv_appendentry_msg(r_ctx
            , peer
            , append_entries_req->leader_id
            , append_entries_req->leader_term
            , append_entries_req->leader_committed_index
            , append_entries_req->prev_log_idx
            , append_entries_req->prev_log_term
            , append_entries_req->n_append_entry_arr
            , append_entries_req->append_entry_arr);

    return ;
}

void r_recv_append_entries_rsp(raft_peer_t* peer, const RAFTPKG* pkg)
{
    assert_retnone(NULL != peer && NULL != pkg);

    LOGD("r_recv_append_entries_rsp");

    PKGAPPENDENTRIESRSP *append_entries_rsp = pkg->append_entries_rsp;
    raft_context_t* r_ctx = r_ctx_get_raft();
    assert_retnone(NULL != append_entries_rsp && NULL != r_ctx);

    raft_fsm_msg_push_recv_appendentry_rsp_msg(r_ctx
            , peer
            , append_entries_rsp->ret
            , append_entries_rsp->peer_term
            , append_entries_rsp->peer_match_index);

    return ;
}

