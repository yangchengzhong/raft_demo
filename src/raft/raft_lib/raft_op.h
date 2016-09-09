#ifndef __RAFT_OP_H__
#define __RAFT_OP_H__

#include "raft_context.h"

int raft_op_start_election(raft_context_t* raft_ctx);
void raft_op_stop_election(raft_context_t* raft_ctx);
int raft_op_restart_election(raft_context_t* raft_ctx);

int raft_op_start_appendentry_timer(raft_context_t* raft_ctx);
void raft_op_stop_appendentry_timer(raft_context_t* raft_ctx);
int raft_op_restart_appendentry_timer(raft_context_t* raft_ctx);

int raft_op_append_entry(raft_context_t* r_ctx, raft_entry_t* entry);

void raft_op_mark_peers_index(raft_context_t* raft_ctx);

void raft_op_clear_voted_info(raft_context_t* raft_ctx);

#endif

