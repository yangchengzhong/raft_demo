#ifndef __RAFT_SEND_H__
#define __RAFT_SEND_H__

#include "common_inc.h"
#include "raft_context.h"
#include "raft_conn.h"

int raft_send_vote_req(raft_context_t* raft_ctx, raft_conn_t* conn, u64 self_id, int cur_term, u64 last_entry_term, u64 last_entry_idx); 

int raft_send_vote_rsp(raft_context_t* raft_ctx, raft_conn_t* conn, u64 self_id, u64 cur_term, int ret);

int raft_send_append_entries(raft_context_t* raft_ctx, raft_peer_t* peer, raft_conn_t* conn);

int raft_send_append_entry_rsp(raft_context_t* raft_ctx, raft_conn_t* conn, int ret, u64 cur_term, u64 match_index);

#endif

