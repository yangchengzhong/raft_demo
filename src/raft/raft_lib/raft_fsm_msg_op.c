#include "raft_fsm_msg_op.h"

#include "state_machine.h"
#include "raft_fsm_meta.h"
#include "raft_fsm_def.h"

static int _push_msg(E_RAFT_FSM_MSG msg, state_machine_t* fsm_inst, raft_fsm_cb_data_t *cb_data);

int raft_fsm_msg_push_election_timeout_msg(raft_context_t* raft_ctx)
{  
    assert_retval(NULL != raft_ctx, ERR_INVALID_ARG);

    raft_fsm_cb_data_t cb_data;
    cb_data.msg = E_RAFT_FSM_MSG_ELECTION_TIMEOUT;

    return _push_msg(cb_data.msg, &raft_ctx->sm_inst, &cb_data);
}

int raft_fsm_msg_push_vote_req_msg(raft_context_t* raft_ctx, raft_peer_t* peer, u64 reqer_id, u64 reqer_term, u64 last_entry_term, u64 last_entry_idx)
{
    assert_retval(NULL != raft_ctx && NULL != peer, ERR_INVALID_ARG);

    raft_fsm_cb_data_t cb_data;
    cb_data.msg = E_RAFT_FSM_MSG_VOTE_REQ;
    cb_data.fsm_cb_un.vote_req.peer = peer;
    cb_data.fsm_cb_un.vote_req.reqer_id = reqer_id;
    cb_data.fsm_cb_un.vote_req.reqer_term = reqer_term;
    cb_data.fsm_cb_un.vote_req.last_entry_term = last_entry_term;
    cb_data.fsm_cb_un.vote_req.last_entry_idx = last_entry_idx;

    return _push_msg(cb_data.msg, &raft_ctx->sm_inst, &cb_data);
}

int raft_fsm_msg_push_vote_rsp_msg(raft_context_t* raft_ctx, raft_peer_t* peer, int ret, u64 target_id, u64 target_term)
{
    assert_retval(NULL != raft_ctx && NULL != peer, ERR_INVALID_ARG);

    raft_fsm_cb_data_t cb_data;
    cb_data.msg = E_RAFT_FSM_MSG_VOTE_RSP;
    cb_data.fsm_cb_un.vote_rsp.peer = peer;
    cb_data.fsm_cb_un.vote_rsp.ret = ret;
    cb_data.fsm_cb_un.vote_rsp.target_id = target_id;
    cb_data.fsm_cb_un.vote_rsp.target_term = target_term;

    return _push_msg(cb_data.msg, &raft_ctx->sm_inst, &cb_data);
}

int raft_fsm_msg_push_recv_appendentry_msg(raft_context_t* raft_ctx
        , raft_peer_t* peer
        , u64 leader_id
        , u64 leader_term
        , u64 leader_committed_index
        , u64 prev_log_idx
        , u64 prev_log_term
        , int n_append_entry_arr
        , APPENDENTRY* append_entry_arr[])
{
    assert_retval(NULL != raft_ctx && NULL != peer, ERR_INVALID_ARG);

    raft_fsm_cb_data_t cb_data;
    cb_data.msg = E_RAFT_FSM_MSG_RECV_ENTRY;
    raft_fsm_cb_entry_req_t* entry_req_un = &cb_data.fsm_cb_un.entry_req;
    entry_req_un->peer = peer;
    entry_req_un->leader_id = leader_id;
    entry_req_un->leader_term = leader_term;
    entry_req_un->leader_committed_index = leader_committed_index;
    entry_req_un->prev_log_idx = prev_log_idx;
    entry_req_un->prev_log_term = prev_log_term;
    entry_req_un->n_append_entry_arr = n_append_entry_arr;
    entry_req_un->append_entry_arr = append_entry_arr;

    return _push_msg(cb_data.msg, &raft_ctx->sm_inst, &cb_data);
}

int raft_fsm_msg_push_on_entry_msg(raft_context_t* raft_ctx, ds_t* entry_content)
{
    assert_retval(NULL != raft_ctx && NULL != entry_content, ERR_INVALID_ARG);

    raft_fsm_cb_data_t cb_data;
    cb_data.msg = E_RAFT_FSM_MSG_RECV_ENTRY;
    cb_data.fsm_cb_un.on_entry.entry_content = entry_content;

    return _push_msg(cb_data.msg, &raft_ctx->sm_inst, &cb_data);
}

int raft_fsm_msg_push_append_entry_timeout_msg(raft_context_t* raft_ctx)
{
    assert_retval(NULL != raft_ctx, ERR_INVALID_ARG);

    raft_fsm_cb_data_t cb_data;
    cb_data.msg = E_RAFT_FSM_MSG_APPENDENTRY_TIMEOUT;

    return _push_msg(cb_data.msg, &raft_ctx->sm_inst, &cb_data);
}

int raft_fsm_msg_push_recv_appendentry_rsp_msg(raft_context_t* raft_ctx
        , raft_peer_t* peer
        , int ret
        , u64 peer_term
        , u64 peer_match_index)
{
    assert_retval(NULL != raft_ctx && NULL != peer, ERR_INVALID_ARG);

    raft_fsm_cb_data_t cb_data;
    cb_data.msg = E_RAFT_FSM_MSG_RECV_ENTRY_RSP;
    cb_data.fsm_cb_un.entry_rsp.ret = ret;
    cb_data.fsm_cb_un.entry_rsp.peer = peer;
    cb_data.fsm_cb_un.entry_rsp.peer_term = peer_term;
    cb_data.fsm_cb_un.entry_rsp.peer_match_index = peer_match_index;

    return _push_msg(cb_data.msg, &raft_ctx->sm_inst, &cb_data);
}

/*********************** static functions ************************/
static int _push_msg(E_RAFT_FSM_MSG msg, state_machine_t* fsm_inst, raft_fsm_cb_data_t *cb_data)
{  
    LOGD("fsm on push msg[%d]", msg);

    return state_machine_msg_dealer(raft_fsm_get_meta()
            , fsm_inst
            , msg
            , cb_data
            , sizeof(*cb_data));
}

