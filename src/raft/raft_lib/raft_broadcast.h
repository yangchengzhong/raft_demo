#ifndef __RAFT_BROADCAST_H__
#define __RAFT_BROADCAST_H__

#include "common_inc.h"
#include "raft_context.h"

int raft_broadcast_vote_req(raft_context_t* raft_ctx, u64 self_id, int cur_term);

int raft_broadcast_append_entry(raft_context_t* r_ctx);

#endif

