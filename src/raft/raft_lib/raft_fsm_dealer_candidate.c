#include "raft_fsm_dealer_candidate.h"

#include "raft_context.h"
#include "raft_op.h"
#include "raft_fsm_def.h"
#include "raft_broadcast.h"
#include "raft_send.h"
#include "raft_fsm_dealer_check.h"
#include "raft_fsm_dealer_op.h"
#include "util_math.h"
#include "raft_fsm_dealer_.h"

static int _candidate_deal_election_timeout(raft_context_t* r_ctx, void *msg_data, int msg_data_size, bool *need_change_state, int *next_state);
static int _candidate_deal_vote_rsp(raft_context_t* r_ctx, void *msg_data, int msg_data_size, bool *need_change_state, int *next_state);

int raft_fsm_candidate_state_dealer(void *fsm_holder, int msg, void *msg_data, int msg_data_size, bool *need_change_state, int *next_state)
{
    assert_retval(NULL != fsm_holder 
            && NULL != msg_data 
            && msg_data_size >= 0
            && NULL != need_change_state
            && NULL != next_state
            , ERR_INVALID_ARG);

    raft_context_t* r_ctx = (raft_context_t*)fsm_holder;
    assert_retval(NULL != r_ctx, ERR_INVALID_ARG);

    int ret = ERR_INIT;

    switch(msg)
    {  
        case E_RAFT_FSM_MSG_ELECTION_TIMEOUT:
            ret = _candidate_deal_election_timeout(r_ctx, msg_data, msg_data_size, need_change_state, next_state);
            break;

        case E_RAFT_FSM_MSG_VOTE_REQ:
            ret = raft_fsm_dealer_vote_req_for_term(r_ctx, msg_data, msg_data_size, need_change_state, next_state);
            break;

        case E_RAFT_FSM_MSG_VOTE_RSP:
            ret = _candidate_deal_vote_rsp(r_ctx, msg_data, msg_data_size, need_change_state, next_state);
            break;

        case E_RAFT_FSM_MSG_ON_ENTRY:
            ret = raft_fsm_dealer_on_entry_redirect(r_ctx, msg_data, msg_data_size, need_change_state, next_state);
            break;
    
        case E_RAFT_FSM_MSG_RECV_ENTRY:
            ret = raft_fsm_dealer_on_recv_entry(r_ctx, msg_data, msg_data_size, need_change_state, next_state);
            break;

        default:
            LOGI("raft_fsm_candidate_state_dealer on msg[%d]", msg);
            break;
    
    }

    //LOGD("candidate_state_dealer, msg[%d], ret[%d]", msg, ret);

    return ret;
}

void raft_fsm_candidate_enter_action(void *fsm_holder, void* msg_data, int msg_data_size)
{
    raft_context_t* raft_ctx = (raft_context_t*)fsm_holder;
    assert_retnone(NULL != raft_ctx);
    
    raft_ctx_inc_term(raft_ctx);

    raft_ctx_set_vote_count(raft_ctx, 1);

    raft_ctx_set_candidate_id(raft_ctx, raft_ctx_get_id(raft_ctx));

    raft_broadcast_vote_req(raft_ctx
            , raft_ctx_get_id(raft_ctx)
            , raft_ctx_get_term(raft_ctx));

    return ;
}

void raft_fsm_candidate_leave_action(void *fsm_holder, void* msg_data, int msg_data_size)
{
    /* stop election */
    raft_context_t* raft_ctx = (raft_context_t*)fsm_holder;
    assert_retnone(NULL != raft_ctx);

    raft_op_clear_voted_info(raft_ctx);

    raft_op_stop_election(raft_ctx);

    return ;
}

/*********************** static functions ************************/
static int _candidate_deal_election_timeout(raft_context_t* r_ctx, void *msg_data, int msg_data_size, bool *need_change_state, int *next_state)
{
    *need_change_state = true;
    *next_state = E_RAFT_FSM_STATE_CANDIDATE;

    return 0;
}

static int _candidate_deal_vote_rsp(raft_context_t* r_ctx, void *msg_data, int msg_data_size, bool *need_change_state, int *next_state)
{
    raft_fsm_cb_data_t* cb_data = (raft_fsm_cb_data_t*)msg_data;
    const raft_fsm_cb_vote_rsp_t* vote_rsp = &cb_data->fsm_cb_un.vote_rsp;

    //vote_rsp term new, we're out of date
    if(vote_rsp->target_term > raft_ctx_get_term(r_ctx))
    {
        raft_fsm_dealder_op_prepare_2_follower(&cb_data->fsm_result_un.tofollower
                , vote_rsp->target_term
                , INVALID64
                , INVALID64
                , need_change_state
                , next_state);
        return 0;
    }

    //our term new, this pkg is ood during cast
    if(vote_rsp->target_term < raft_ctx_get_term(r_ctx)) return 0;

    //our req is reject
    if(0 != vote_rsp->ret) return 0; 

    raft_node_t* peer_node = raft_ctx_find_node(r_ctx, vote_rsp->target_id);
    if(NULL == peer_node)
    {
        LOGI("self does not contain peer info[%llu]", vote_rsp->target_id);
        return ERR_NOTFOUND;
    }

    if(!raft_node_flag_contains(peer_node, E_RAFT_NODE_VOTE_FOR_HOLDER))
    {
        raft_node_flag_add(peer_node, E_RAFT_NODE_VOTE_FOR_HOLDER);
        raft_ctx_inc_vote_count(r_ctx);
    }

    if(raft_ctx_is_majority(r_ctx, raft_ctx_get_vote_count(r_ctx)))
    {
        *need_change_state = true;
        *next_state = E_RAFT_FSM_STATE_LEADER;
    }

    return 0;
}

