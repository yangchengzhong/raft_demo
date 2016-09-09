#include "raft_fsm_dealer_follower.h"

#include "raft_context.h"
#include "raft_op.h"
#include "raft_fsm_def.h"
#include "raft_send.h"
#include "raft_fsm_dealer_check.h"
#include "raft_fsm_dealer_op.h"
#include "util_math.h"
#include "raft_fsm_dealer_.h"

static int _follower_deal_election_timeout(raft_context_t* r_ctx, void *msg_data, int msg_data_size, bool *need_change_state, int *next_state);
static int _follower_deal_vote_req(raft_context_t* r_ctx, void *msg_data, int msg_data_size, bool *need_change_state, int *next_state);

int raft_fsm_follower_state_dealer(void* fsm_holder, int msg, void *msg_data, int msg_data_size, bool *need_change_state, int *next_state)
{
    assert_retval(NULL != fsm_holder 
            && NULL != msg_data 
            && msg_data_size >= 0
            && NULL != need_change_state
            && NULL != next_state
            , ERR_INVALID_ARG);

    raft_context_t* r_ctx = (raft_context_t*)fsm_holder;

    int ret = ERR_INIT;

    switch(msg)
    {  
        case E_RAFT_FSM_MSG_ELECTION_TIMEOUT:
            ret = _follower_deal_election_timeout(r_ctx, msg_data, msg_data_size, need_change_state, next_state);
            break;

        case E_RAFT_FSM_MSG_VOTE_REQ:
            ret = _follower_deal_vote_req(r_ctx, msg_data, msg_data_size, need_change_state, next_state);
            break;

        case E_RAFT_FSM_MSG_ON_ENTRY:
            ret = raft_fsm_dealer_on_entry_redirect(r_ctx, msg_data, msg_data_size, need_change_state, next_state);
            break;

        case E_RAFT_FSM_MSG_RECV_ENTRY:
            ret = raft_fsm_dealer_on_recv_entry(r_ctx, msg_data, msg_data_size, need_change_state, next_state);
            break;
    
        default:
            LOGI("raft_fsm_follower_state_dealer on msg[%d]", msg);
            break;
    
    }

    //LOGD("follower_state_dealer, msg[%d], ret[%d]", msg, ret);

    return ret;
}

void raft_fsm_follower_enter_action(void* fsm_holder, void* msg_data, int msg_data_size)
{
    assert_retnone(NULL != fsm_holder && NULL != msg_data && msg_data_size >= 0);

    raft_context_t* raft_ctx = (raft_context_t*)fsm_holder;
    assert_retnone(NULL != raft_ctx);

    /* state */
    raft_fsm_result_tofollower_t *tofollower = &((raft_fsm_cb_data_t*)msg_data)->fsm_result_un.tofollower;
    raft_ctx_set_term(raft_ctx, tofollower->term);
    raft_ctx_set_candidate_id(raft_ctx, tofollower->candidate_id);
    raft_ctx_set_leader_id(raft_ctx, tofollower->leader_id);

    /* start election */
    int ret = raft_op_start_election(raft_ctx);
    if(0 != ret)
    {
        LOGE("fail to start election, ret = %d", ret);
    }

    return ;
}

void raft_fsm_follower_leave_action(void* fsm_holder, void* msg_data, int msg_data_size)
{
    /* stop election */
    raft_context_t* raft_ctx = (raft_context_t*)fsm_holder;
    assert_retnone(NULL != raft_ctx);

    raft_op_stop_election(raft_ctx);

    return ;
}

/*********************** static functions ************************/
static int _follower_deal_election_timeout(raft_context_t* r_ctx, void *msg_data, int msg_data_size, bool *need_change_state, int *next_state)
{
    *need_change_state = true;
    *next_state = E_RAFT_FSM_STATE_CANDIDATE;

    return 0;
}

static int _follower_deal_vote_req(raft_context_t* r_ctx, void *msg_data, int msg_data_size, bool *need_change_state, int *next_state)
{
    raft_fsm_cb_data_t* cb_data = (raft_fsm_cb_data_t*)msg_data;
    const raft_fsm_cb_vote_req_t* vote_req = &cb_data->fsm_cb_un.vote_req;
    assert_retval(NULL != vote_req->peer, ERR_INVALID_ARG);

    int ret = ERR_INIT;

    do
    {
        /* check if comming term too low */
        ret = raft_fsm_dealer_check_peerterm_ge(r_ctx, vote_req->reqer_term);
        if(0 != ret) break;

        /* check if has vote */
        ret = raft_fsm_dealer_check_has_vote(r_ctx, vote_req);
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

        raft_node_flag_add(&r_ctx->self_node, E_RAFT_NODE_HAS_VOTE_OTHER);

    }while(0);
    
    raft_send_vote_rsp(r_ctx
            , &vote_req->peer->conn
            , raft_ctx_get_id(r_ctx)
            , MAX(vote_req->reqer_term, raft_ctx_get_term(r_ctx))
            , ret);

    return ret;
}

