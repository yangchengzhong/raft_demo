#include "raft_broadcast.h"

#include "raft_send.h"

int raft_broadcast_vote_req(raft_context_t* raft_ctx, u64 self_id, int cur_term)
{  
    assert_retval(NULL != raft_ctx, ERR_INVALID_ARG);
    
    skiplist_node_t* cur = NULL;

    raft_entry_t* tail_entry = raft_ctx_get_tail_entry(raft_ctx); 
    u64 tail_entry_idx = NULL == tail_entry ? INVALID64 : raft_ctx_get_entry_tail_idx(raft_ctx);
    u64 tail_entry_term = NULL == tail_entry ? INVALID64 : tail_entry->term;

    SKIPLIST_FOR_EACH(raft_ctx->peer_sklist, cur)
    {
        raft_peer_t* peer = cur->next[1]->value;
        assert_continue(NULL != peer);

        if(!raft_node_flag_contains(&peer->node, E_RAFT_NODE_PATICIPATE_VOTING)) continue;

        raft_send_vote_req(raft_ctx, &peer->conn, self_id, cur_term, tail_entry_term, tail_entry_idx);
    }

    return 0;
}

int raft_broadcast_append_entry(raft_context_t* raft_ctx)
{
    assert_retval(NULL != raft_ctx, ERR_INVALID_ARG);

    skiplist_node_t* cur = NULL;

    SKIPLIST_FOR_EACH(raft_ctx->peer_sklist, cur)
    {
        raft_peer_t* peer = cur->next[1]->value;
        assert_continue(NULL != peer);

        raft_send_append_entries(raft_ctx, peer, &peer->conn);
    }

    return 0;
}

