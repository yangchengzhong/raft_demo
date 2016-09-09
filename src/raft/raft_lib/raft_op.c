#include "raft_op.h"

#include "util_math.h"
#include "raft_timeout.h"
#include "timer_util.h"
#include "r_cfg_global.h"
#include "raft_cfg.h"
#include "util.h"

static int _decide_election_msecs();

int raft_op_start_election(raft_context_t* raft_ctx)
{
    assert_retval(NULL != raft_ctx, ERR_INVALID_ARG);

    /* random time to wait */
    raft_ctx->election_timeout_timestamp = _decide_election_msecs() + util_now();

    return 0;
}

void raft_op_stop_election(raft_context_t* raft_ctx)
{
    /* remove timer */
    assert_retnone(NULL != raft_ctx);

    raft_ctx->election_timeout_timestamp = UINT64_MAX;

    return ;
}

int raft_op_restart_election(raft_context_t* raft_ctx)
{
    assert_retval(NULL != raft_ctx, ERR_INVALID_ARG);

    raft_op_stop_election(raft_ctx);

    raft_op_start_election(raft_ctx);

    return 0;
}

void raft_op_mark_peers_index(raft_context_t* raft_ctx)
{
    assert_retnone(NULL != raft_ctx);

    u64 self_log_idx = raft_ctx_get_entry_tail_idx(raft_ctx);
    u64 next_idx = INVALID64 == self_log_idx ? INVALID64 : self_log_idx + 1;
    u64 match_idx = INVALID64;

    skiplist_node_t* cur = NULL;

    SKIPLIST_FOR_EACH(raft_ctx->peer_sklist, cur)
    {
        raft_peer_t* peer = cur->next[1]->value;
        assert_continue(NULL != peer);

        raft_peer_set_next_index(peer, next_idx);
        raft_peer_set_match_index(peer, match_idx);
    }

    return ;
}

int raft_op_start_appendentry_timer(raft_context_t* raft_ctx)
{
    assert_retval(NULL != raft_ctx, ERR_INVALID_ARG);

    raft_cfg_t* cfg = r_cfg_get_raft_cfg();
    assert_retval(NULL != cfg, ERR_INVALID_ARG);

    raft_ctx->appendentry_timeout_timestamp = util_now() + cfg->append_entry_timeout_msec;

    return 0;
}

void raft_op_stop_appendentry_timer(raft_context_t* raft_ctx)
{
    assert_retnone(NULL != raft_ctx);

    raft_ctx->appendentry_timeout_timestamp = UINT64_MAX;

    return ;
}

int raft_op_restart_appendentry_timer(raft_context_t* raft_ctx)
{
    assert_retval(NULL != raft_ctx, ERR_INVALID_ARG);

    raft_op_stop_appendentry_timer(raft_ctx);

    raft_op_start_appendentry_timer(raft_ctx);

    return 0;
}

int raft_op_append_entry(raft_context_t* r_ctx, raft_entry_t* entry)
{
    assert_retval(NULL != r_ctx && NULL != entry, ERR_INVALID_ARG);

    return raft_ctx_put_entry(r_ctx, entry);
}

void raft_op_clear_voted_info(raft_context_t* raft_ctx)
{
    /* clear vote info */
    raft_ctx_set_vote_count(raft_ctx, 0);

    skiplist_node_t* cur = NULL;

    SKIPLIST_FOR_EACH(raft_ctx->peer_sklist, cur)
    {
        raft_peer_t* peer = cur->next[1]->value;
        assert_continue(NULL != peer);

        raft_node_flag_remove(&peer->node, E_RAFT_NODE_VOTE_FOR_HOLDER);
    }

    return ;
}

/*********************** static functions ************************/
static int _decide_election_msecs()
{
    raft_cfg_t* cfg = r_cfg_get_raft_cfg();
    assert_retval(NULL != cfg, ERR_INVALID_ARG);

    return util_rand(cfg->election_timeout_min_msec, cfg->election_timeout_max_msec); 
}

