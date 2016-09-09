#include "raft_fsm_dealer_leader.h"

#include "raft_context.h"
#include "raft_fsm_def.h" 
#include "raft_fsm_dealer_check.h"
#include "raft_fsm_dealer_op.h"
#include "raft_send.h"
#include "raft_fsm_dealer_.h"
#include "util_math.h"
#include "raft_op.h"
#include "raft_broadcast.h"

static int _leader_deal_on_entry(raft_context_t* r_ctx, void *msg_data, int msg_data_size, bool *need_change_state, int *next_state);
static int _leader_deal_appendentry_timeout(raft_context_t* r_ctx, void *msg_data, int msg_data_size, bool *need_change_state, int *next_state);
static int _leader_deal_entry_rsp(raft_context_t* r_ctx, void *msg_data, int msg_data_size, bool *need_change_state, int *next_state);

int raft_fsm_leader_state_dealer(void *fsm_holder, int msg, void *msg_data, int msg_data_size, bool *need_change_state, int *next_state)
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
        case E_RAFT_FSM_MSG_VOTE_REQ:
            ret = raft_fsm_dealer_vote_req_for_term(r_ctx, msg_data, msg_data_size, need_change_state, next_state);
            break;

        case E_RAFT_FSM_MSG_ON_ENTRY:
            ret = _leader_deal_on_entry(r_ctx, msg_data, msg_data_size, need_change_state, next_state);
            break;
        
        case E_RAFT_FSM_MSG_APPENDENTRY_TIMEOUT:
            ret = _leader_deal_appendentry_timeout(r_ctx, msg_data, msg_data_size, need_change_state, next_state);
            break;

        case E_RAFT_FSM_MSG_RECV_ENTRY:
        {
            assert_retval(raft_ctx_get_term(r_ctx) != ((raft_fsm_cb_data_t*)msg_data)->fsm_cb_un.entry_req.leader_term, ERR_INTERNAL);
            ret = raft_fsm_dealer_on_recv_entry(r_ctx, msg_data, msg_data_size, need_change_state, next_state);
            break;
        }

        case E_RAFT_FSM_MSG_RECV_ENTRY_RSP:
            ret = _leader_deal_entry_rsp(r_ctx, msg_data, msg_data_size, need_change_state, next_state);
            break;

        default:
            LOGI("raft_fsm_leader_state_dealer on msg[%d]", msg);
            break;
    
    }

    //LOGD("leader_state_dealer, msg[%d], ret[%d]", msg, ret);

    return ret;
}

void raft_fsm_leader_leave_action(void *fsm_holder, void *msg_data, int msg_data_size)
{
    assert_retnone(NULL != fsm_holder && NULL != msg_data && msg_data_size >= 0);

    raft_context_t* raft_ctx = (raft_context_t*)fsm_holder;
    assert_retnone(NULL != raft_ctx);

    raft_op_stop_appendentry_timer(raft_ctx);

    return ;
}

void raft_fsm_leader_enter_action(void *fsm_holder, void *msg_data, int msg_data_size)
{
    assert_retnone(NULL != fsm_holder && NULL != msg_data && msg_data_size >= 0);

    raft_context_t* raft_ctx = (raft_context_t*)fsm_holder;
    assert_retnone(NULL != raft_ctx);

    raft_op_mark_peers_index(raft_ctx);

    int ret = raft_op_start_appendentry_timer(raft_ctx);
    assert_retnone(0 == ret);

    return ;
}

/*********************** static functions ************************/
static int _leader_deal_on_entry(raft_context_t* r_ctx, void *msg_data, int msg_data_size, bool *need_change_state, int *next_state)
{
    raft_fsm_cb_data_t* cb_data = (raft_fsm_cb_data_t*)msg_data;
    const raft_fsm_cb_on_entry_t* on_entry = &cb_data->fsm_cb_un.on_entry;
    assert_retval(NULL != on_entry, ERR_INVALID_ARG);

    /* check if can append */
    int ret = raft_fsm_dealer_check_can_leader_append_entry(r_ctx);
    if(0 != ret) return ret;

    /* create entry */
    raft_entry_t* entry = raft_entry_new(raft_ctx_get_term(r_ctx), on_entry->entry_content);
    if(NULL == entry) return ERR_BAD_ALLOC;

    /* exec append */
    ret = raft_op_append_entry(r_ctx, entry);
    if(0 != ret)
    {
        raft_entry_delete(entry);
        return ret;
    }

    return 0;
}

static int _leader_deal_appendentry_timeout(raft_context_t* r_ctx, void *msg_data, int msg_data_size, bool *need_change_state, int *next_state)
{
    int ret = raft_broadcast_append_entry(r_ctx);
    assert_retval(0 == ret, ret);

    ret = raft_op_restart_appendentry_timer(r_ctx);
    assert_retval(0 == ret, ret);

    return 0;
}

static int _leader_deal_entry_rsp_sucessfully(raft_context_t* r_ctx, const raft_fsm_cb_entry_rsp_t* entry_rsp);
static int _leader_deal_entry_rsp_fail(raft_context_t* r_ctx, const raft_fsm_cb_entry_rsp_t* entry_rsp);

static int _leader_deal_entry_rsp(raft_context_t* r_ctx, void *msg_data, int msg_data_size, bool *need_change_state, int *next_state)
{
    raft_fsm_cb_data_t* cb_data = (raft_fsm_cb_data_t*)msg_data;
    const raft_fsm_cb_entry_rsp_t* entry_rsp = &cb_data->fsm_cb_un.entry_rsp;

    /* check if info valid */
    int ret = raft_fsm_dealer_check_entry_rsp(r_ctx, entry_rsp->peer, entry_rsp->peer_term, entry_rsp->peer_match_index);
    if(0 != ret) return ret;

    /* check if term valid */
    ret = raft_fsm_dealer_check_peerterm_le(r_ctx, entry_rsp->peer_term);
    if(0 != ret)
    {
        raft_fsm_dealder_op_prepare_2_follower(&cb_data->fsm_result_un.tofollower
                , entry_rsp->peer_term
                , INVALID64
                , INVALID64
                , need_change_state
                , next_state);
        return ret;
    }

    /* exec handle */
    return entry_rsp->ret == 0 
        ? _leader_deal_entry_rsp_sucessfully(r_ctx, entry_rsp)
        : _leader_deal_entry_rsp_fail(r_ctx, entry_rsp);
}

static int _leader_deal_entry_rsp_sucessfully(raft_context_t* r_ctx, const raft_fsm_cb_entry_rsp_t* entry_rsp)
{
    //if match idx invaid64 while success,means heartbeat
    //todo need refactor for msg type

    //if successfully, match index will >= perv_log_idx
    /* set peer index info */
    u64 match_idx = entry_rsp->peer_match_index;
    if(INVALID64 == match_idx) return 0;

    raft_peer_set_next_index(entry_rsp->peer, match_idx + 1);
    raft_peer_set_match_index(entry_rsp->peer, match_idx);

    /* try set self commit idx */
    if(raft_ctx_get_committed_index(r_ctx) >= match_idx) return 0;

    skiplist_node_t* cur = NULL;

    int commited_count = 1;//for self

    SKIPLIST_FOR_EACH(r_ctx->peer_sklist, cur)
    {
        raft_peer_t* peer = cur->next[1]->value;
        assert_continue(NULL != peer);

        u64 iter_match_idx = raft_peer_get_match_index(peer);

        if(INVALID64 != iter_match_idx && iter_match_idx >= match_idx) ++commited_count;
    }

    if(raft_ctx_is_majority(r_ctx, commited_count))
    {
        raft_ctx_set_committed_index(r_ctx, match_idx); 
    }

    return 0;
}

//here ,we retry and minus the idx
static int _leader_deal_entry_rsp_fail(raft_context_t* r_ctx, const raft_fsm_cb_entry_rsp_t* entry_rsp)
{
    //heartbeat should not fail unless term issue which has been filter before
    raft_peer_t* peer = entry_rsp->peer;

    u64 next_idx = raft_peer_get_next_index(peer);

    //invalid means no log,should not proc here
    //zero means send fail, wait for heartbeat to resend
    //no entry,no need to resend
    if(INVALID64 == next_idx || 0 == next_idx || INVALID64 == raft_ctx_get_entry_tail_idx(r_ctx)) return 0;

    raft_peer_set_next_index(peer, next_idx - 1);

    raft_send_append_entries(r_ctx, peer, &peer->conn);

    return 0;
}
