#include "raft_fsm_dealer_.h"

#include "raft_fsm_def.h"
#include "raft_fsm_dealer_check.h"
#include "raft_fsm_dealer_op.h"
#include "raft_send.h"
#include "util_math.h"

int raft_fsm_dealer_vote_req_for_term(raft_context_t* r_ctx
        , void *msg_data
        , int msg_data_size
        , bool *need_change_state
        , int *next_state)
{
    raft_fsm_cb_data_t* cb_data = (raft_fsm_cb_data_t*)msg_data;
    const raft_fsm_cb_vote_req_t* vote_req = &cb_data->fsm_cb_un.vote_req;
    assert_retval(NULL != vote_req->peer, ERR_INVALID_ARG);

    int ret = ERR_INIT;

    do
    {
        ret = raft_fsm_dealer_check_peerterm_gt(r_ctx, vote_req->reqer_term);
        if(0 != ret) break;

        ret = raft_fsm_dealer_check_entry_in_date(r_ctx, vote_req->last_entry_term, vote_req->last_entry_idx);
        if(0 != ret) break;

        ret = raft_fsm_dealder_op_prepare_2_follower(&cb_data->fsm_result_un.tofollower
                , vote_req->reqer_term
                , vote_req->reqer_id
                , INVALID64
                , need_change_state
                , next_state);
        if(0 != ret) break;

    }while(0);

    raft_send_vote_rsp(r_ctx
            , &vote_req->peer->conn
            , raft_ctx_get_id(r_ctx)
            , MAX(vote_req->reqer_term, raft_ctx_get_term(r_ctx))
            , ret);

    return ret;
}

int raft_fsm_dealer_on_entry_redirect(raft_context_t* r_ctx
        , void *msg_data
        , int msg_data_size
        , bool *need_change_state
        , int *next_state)
{
    *need_change_state = false;
    return ERR_RAFT_LEADER_REDIRECT;
}

static int _on_recv_entry_proc(raft_context_t* r_ctx, raft_fsm_cb_data_t* cb_data, bool *need_change_state, int *next_state)
{
    const raft_fsm_cb_entry_req_t* entry_req = &cb_data->fsm_cb_un.entry_req;
    assert_retval(NULL != entry_req->peer, ERR_INVALID_ARG);

    int ret = ERR_INIT;

    do
    {
        if(0 == entry_req->n_append_entry_arr)
        {
            ret = 0;
            break;// for heartbeat pkg
        }

        /* check log index valid */
        assert_noeffect(INVALID64 != entry_req->prev_log_idx);
        ret = raft_fsm_dealer_check_prev_log_exist(r_ctx, entry_req->prev_log_idx);
        if(0 != ret) break;

        /* try delete out-of-date log */
        ret = raft_fsm_dealer_op_try_remove_by_prev(r_ctx, entry_req->n_append_entry_arr, entry_req->prev_log_idx, entry_req->prev_log_term);
        if(0 != ret) break;

        /* append log */
        ret = raft_fsm_dealer_op_append_entries(r_ctx
                , entry_req->prev_log_idx
                , entry_req->n_append_entry_arr
                , entry_req->append_entry_arr);
        assert_noeffect(0 == ret);
        if(0 != ret) break;

        /* update commit idx */
        raft_fsm_dealer_op_update_commit_idx(r_ctx, entry_req->leader_committed_index);

    }while(0);

    /* prepare to follower(refresh timer) */
    raft_fsm_dealder_op_prepare_2_follower(&cb_data->fsm_result_un.tofollower
            , entry_req->leader_term
            , INVALID64
            , entry_req->leader_id
            , need_change_state
            , next_state);

    return ret;
}
int raft_fsm_dealer_on_recv_entry(raft_context_t* r_ctx
        , void *msg_data
        , int msg_data_size
        , bool *need_change_state
        , int *next_state)
{
    raft_fsm_cb_data_t* cb_data = (raft_fsm_cb_data_t*)msg_data;
    const raft_fsm_cb_entry_req_t* entry_req = &cb_data->fsm_cb_un.entry_req;
    assert_retval(NULL != entry_req->peer, ERR_INVALID_ARG);

    int ret = ERR_INIT;

    do
    {
        /* check term firstly, equal when leader state assert before */ 
        ret = raft_fsm_dealer_check_peerterm_ge(r_ctx, entry_req->leader_term);
        if(0 != ret) break;

        ret = _on_recv_entry_proc(r_ctx, cb_data, need_change_state, next_state);
        if(0 != ret) break;

    }while(0);

    raft_send_append_entry_rsp(r_ctx
            , &entry_req->peer->conn
            , ret
            , MAX(entry_req->leader_term, raft_ctx_get_term(r_ctx))
            , INVALID64 == entry_req->prev_log_idx ? INVALID64 : entry_req->prev_log_idx + entry_req->n_append_entry_arr);

    return 0;   
}

