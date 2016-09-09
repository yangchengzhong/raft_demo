#include "raft_context.h"

#include "common_inc.h"
#include "r_cfg_global.h"
#include "util.h"
#include "raft_node.h"
#include "skip_list.h"
#include "raft_peer.h"
#include "raft_util.h"
#include "state_machine.h"
#include "raft_fsm_def.h"
#include "raft_fsm_meta.h"
#include "timer_util.h"
#include "vector.h"

static int _init_peer_info(raft_context_t* ctx);

int raft_ctx_init(raft_context_t* ctx, void* cb_handler, const raft_cb_t* cb)
{
    assert_retval(NULL != ctx && NULL != cb_handler && NULL != cb, ERR_INVALID_ARG);

    r_global_cfg_t* cfg = r_cfg_get_global_cfg();
    assert_retval(NULL != cfg, ERR_NOTFOUND);

    /* member value */
    ctx->cur_term = 0;
    ctx->vote_count = 0;
    ctx->log_vec = vector_new();
    assert_retval(NULL != ctx->log_vec, ERR_BAD_ALLOC);
    vector_set_release_handler(ctx->log_vec, (func_vector_release_f)raft_entry_delete);

    ctx->committed_index = INVALID64;
    ctx->last_appplied = INVALID64;
    ctx->leader_id = INVALID64;
    ctx->election_timeout_timestamp = UINT64_MAX;
    ctx->appendentry_timeout_timestamp = UINT64_MAX;

    /* fsm */
    raft_fsm_cb_data_t cb_data;
    cb_data.fsm_result_un.tofollower.term = 0;
    cb_data.fsm_result_un.tofollower.candidate_id = INVALID64;
    cb_data.fsm_result_un.tofollower.leader_id = INVALID64;
    state_machine_inst_init(raft_fsm_get_meta()
            , &ctx->sm_inst
            , E_RAFT_FSM_STATE_FOLLOWER
            , &cb_data
            , sizeof(cb_data));

    /* node */
    int ret = raft_node_init(&ctx->self_node
            , raft_id_getter(cfg->listeners[0].ip_str, cfg->listeners[0].port)
            , E_RAFT_NODE_PATICIPATE_VOTING);
    assert_retval(0 == ret, ret);

    /* peer */
    ret = _init_peer_info(ctx);
    assert_retval(0 == ret, ret);

    /* loop timer */
    ctx->loop_timer = uv_add_timer(cfg->raft_cfg.loop_timeout_msec
            , cfg->raft_cfg.loop_timeout_msec
            , E_TIMER_RAFT_LOOP
            , ctx
            , NULL);
    assert_retval(NULL != ctx->loop_timer, ERR_BAD_ALLOC);

    /* raft lib cb */
    ctx->biz_cb_handler = cb_handler;
    ctx->biz_cb = *cb;

    return 0;
}

void raft_ctx_inc_term(raft_context_t* ctx)
{  
    assert_retnone(NULL != ctx);

    ++ctx->cur_term;
    return ;
}

void raft_ctx_set_term(raft_context_t* ctx, u64 term)
{
    assert_retnone(NULL != ctx);

    ctx->cur_term = term;

    return ;
}

u64 raft_ctx_get_term(raft_context_t* ctx)
{  
    assert_retval(NULL != ctx, INVALID64);

    return ctx->cur_term;
}

u64 raft_ctx_get_id(raft_context_t* ctx)
{  
    assert_retval(NULL != ctx, INVALID64);

    return ctx->self_node.id;
}

void raft_ctx_inc_vote_count(raft_context_t* ctx)
{  
    assert_retnone(NULL != ctx);

    ++ctx->vote_count;
    return;
}
void raft_ctx_set_vote_count(raft_context_t* ctx, int vote_count)
{  
    assert_retnone(NULL != ctx);

    ctx->vote_count = vote_count; 
    return ;
}
int raft_ctx_get_vote_count(raft_context_t* ctx)
{  
    assert_retval(NULL != ctx, INVALID32);

    return ctx->vote_count;
}

void raft_ctx_set_candidate_id(raft_context_t* ctx, u64 candidate_id)
{
    assert_retnone(NULL != ctx);

    ctx->candidate_id = candidate_id; 

    return ;
}

void raft_ctx_set_leader_id(raft_context_t* ctx, u64 leader_id)
{
    assert_retnone(NULL != ctx);

    ctx->leader_id = leader_id;

    return ;
}

void raft_ctx_set_callback(raft_context_t* ctx, void* cb_handler, const raft_cb_t* cb)
{
    assert_retnone(NULL != ctx && NULL != cb_handler && NULL != cb);

    ctx->biz_cb_handler = cb_handler;
    ctx->biz_cb = *cb;

    return ;
}

raft_peer_t* raft_ctx_find_peer(raft_context_t* raft_ctx, const net_recv_ctx_t* recv_ctx)
{
    assert_retval(NULL != raft_ctx && NULL != recv_ctx, NULL);

    skiplist_node_t* cur = NULL;

    SKIPLIST_FOR_EACH(raft_ctx->peer_sklist, cur)
    {
        raft_peer_t* peer = cur->next[1]->value;
        assert_continue(NULL != peer);
        
        if(util_sockaddr_2_u64(&recv_ctx->net_recv_ctx_un.udp_recv_ctx.addr)
                == util_sockipport_2_u64(peer->conn.ip, peer->conn.port))
        {
            return peer;
        }
    }

    return NULL;
}

raft_node_t* raft_ctx_find_node(raft_context_t* raft_ctx, u64 node_id)
{
    assert_retval(NULL != raft_ctx, NULL);

    skiplist_node_t* cur = NULL;

    SKIPLIST_FOR_EACH(raft_ctx->peer_sklist, cur)
    {
        raft_peer_t* peer = cur->next[1]->value;
        assert_continue(NULL != peer);
        
        if(raft_node_id_get(&peer->node) == node_id) return &peer->node;
    }

    return NULL;
}

int raft_ctx_get_peer_count(raft_context_t* raft_ctx)
{
    assert_retval(NULL != raft_ctx, INVALID64);

    return skiplist_size(raft_ctx->peer_sklist);
}

int raft_ctx_get_leader_id(raft_context_t* raft_ctx)
{
    assert_retval(NULL != raft_ctx, INVALID64);

    return raft_ctx->leader_id;
}

int raft_ctx_put_entry(raft_context_t* raft_ctx, raft_entry_t* entry)
{
    assert_retval(NULL != raft_ctx && NULL != entry, ERR_INVALID_ARG);

    return vector_push_back(raft_ctx->log_vec, entry);
}

void raft_ctx_set_committed_index(raft_context_t* raft_ctx, u64 committed_index)
{
    assert_retnone(NULL != raft_ctx);

    raft_ctx->committed_index = committed_index;

    return ;
}

u64 raft_ctx_get_committed_index(raft_context_t* raft_ctx)
{
    assert_retval(NULL != raft_ctx, INVALID64);
    return raft_ctx->committed_index;
}

raft_entry_t* raft_ctx_get_entry(raft_context_t* raft_ctx, u64 idx)
{
    assert_retval(NULL != raft_ctx, NULL);

    if(INVALID64 == idx) return NULL;

    return vector_get(raft_ctx->log_vec, idx);
}

u64 raft_ctx_get_entry_tail_idx(raft_context_t* raft_ctx)
{
    assert_retval(NULL != raft_ctx, INVALID64);

    return 0 == vector_get_size(raft_ctx->log_vec) ? INVALID64 : vector_get_size(raft_ctx->log_vec);
}

raft_entry_t* raft_ctx_get_tail_entry(raft_context_t* raft_ctx)
{
    int size = vector_get_size(raft_ctx->log_vec);
    if(size <= 0) return NULL;

    return vector_get(raft_ctx->log_vec, size - 1);
}

bool raft_ctx_is_majority(raft_context_t* raft_ctx, u64 number)
{
    assert_retval(NULL != raft_ctx && INVALID64 != number, false);

    return number * 2 > skiplist_size(raft_ctx->peer_sklist) + 1;
}

int raft_ctx_entry_remove_from_idx(raft_context_t* raft_ctx, u64 idx)
{
    assert_retval(NULL != raft_ctx, ERR_INVALID_ARG);

    int size = vector_get_size(raft_ctx->log_vec);
    if(idx >= size) return ERR_INVALID_ARG;
    
    for(int i = 0; i < size - idx; ++i)
    {
        int cur_size = vector_get_size(raft_ctx->log_vec);
        assert_retval(cur_size > 0, ERR_INVALID_ARG);

        int ret = vector_remove(raft_ctx->log_vec, cur_size - 1);
        assert_retval(0 == ret, ret);
    }

    return 0;
}

/*********************** static functions ************************/
static int _init_peer_info(raft_context_t* ctx)
{
    /* init peer container */
    ctx->peer_sklist = skiplist_new();
    if(NULL == ctx->peer_sklist) return ERR_BAD_ALLOC;
    skiplist_set_discard(ctx->peer_sklist, (skiplist_discard_pfn)raft_peer_delete);

    /* read from cfg & addto cont */
    r_global_cfg_t* cfg = r_cfg_get_global_cfg();
    assert_retval(NULL != cfg, ERR_NOTFOUND);

    for(int i = 0; i < cfg->peer_count; ++i)
    {
        raft_peer_t* peer = raft_peer_new(cfg->peer_nodes[i].sock_type
                , cfg->peer_nodes[i].ip_str
                , cfg->peer_nodes[i].port);
        if(NULL == peer) return ERR_BAD_ALLOC;

        int ret = skiplist_put(ctx->peer_sklist, peer->node.id, peer);
        if(0 != ret)
        {
            raft_peer_delete(peer);
            skiplist_destroy(ctx->peer_sklist);
            return ret;
        }
    }

    return 0;
}

