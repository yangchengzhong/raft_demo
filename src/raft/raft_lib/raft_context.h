#ifndef __RAFT_TYPE_H__
#define __RAFT_TYPE_H__

#include "skip_list.h"
#include "raft_node.h"
#include "raft_peer.h"
#include "state_machine_def.h"
#include "raft_callback.h"
#include "vector.h"
#include "raft_entry.h"

typedef struct
{
    u64                 cur_term;
    int                 vote_count;

    //self node
    raft_node_t         self_node;

    //fsm
    state_machine_t     sm_inst;

    //peer info
    skiplist_t*         peer_sklist;

    //timer info
    uv_timer_t*         loop_timer;
    u64                 election_timeout_timestamp;
    u64                 appendentry_timeout_timestamp;

    //cb info
    void*               biz_cb_handler;
    raft_cb_t           biz_cb;

    //follower info
    u64                 candidate_id;
    u64                 leader_id;

    //log info
    vector_t*           log_vec;
    u64                 committed_index;
    u64                 last_appplied;

}raft_context_t;

int raft_ctx_init(raft_context_t* ctx, void* cb_handler, const raft_cb_t* cb);


/* get */
u64 raft_ctx_get_term(raft_context_t* ctx);
u64 raft_ctx_get_id(raft_context_t* ctx);
int raft_ctx_get_vote_count(raft_context_t* ctx);
raft_peer_t* raft_ctx_find_peer(raft_context_t* raft_ctx, const net_recv_ctx_t* recv_ctx);
raft_node_t* raft_ctx_find_node(raft_context_t* raft_ctx, u64 node_id);
int raft_ctx_get_peer_count(raft_context_t* raft_ctx);
int raft_ctx_get_leader_id(raft_context_t* raft_ctx);
u64 raft_ctx_get_committed_index(raft_context_t* raft_ctx);
raft_entry_t* raft_ctx_get_entry(raft_context_t* raft_ctx, u64 idx);
u64 raft_ctx_get_entry_tail_idx(raft_context_t* raft_ctx);
raft_entry_t* raft_ctx_get_tail_entry(raft_context_t* raft_ctx);
bool raft_ctx_is_majority(raft_context_t* raft_ctx, u64 number);

/* set */
void raft_ctx_inc_term(raft_context_t* ctx);
void raft_ctx_set_term(raft_context_t* ctx, u64 term);
void raft_ctx_inc_vote_count(raft_context_t* ctx);
void raft_ctx_set_vote_count(raft_context_t* ctx, int vote_count);
void raft_ctx_set_candidate_id(raft_context_t* ctx, u64 candidate_id);
void raft_ctx_set_callback(raft_context_t* ctx, void* cb_handler, const raft_cb_t* cb);
void raft_ctx_set_committed_index(raft_context_t* raft_ctx, u64 committed_index);
int raft_ctx_put_entry(raft_context_t* raft_ctx, raft_entry_t* entry);
void raft_ctx_set_leader_id(raft_context_t* ctx, u64 leader_id);
int raft_ctx_entry_remove_from_idx(raft_context_t* raft_ctx, u64 idx);

#endif

