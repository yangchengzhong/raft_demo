#ifndef __RAFT_NODE_H__
#define __RAFT_NODE_H__

#include "common_inc.h"

/* node flags */
typedef enum
{
    E_RAFT_NODE_PATICIPATE_VOTING   = 1,
    E_RAFT_NODE_HAS_VOTE_OTHER      = 1<<1,
    E_RAFT_NODE_VOTE_FOR_HOLDER     = 1<<2,
}E_RAFT_NODE_FLAG;

typedef struct
{
    u64 id;
    u64 flags;
}raft_node_t;

int raft_node_init(raft_node_t *node, u64 id, u64 flags);
void raft_node_fini(raft_node_t* node);

bool raft_node_flag_contains(raft_node_t* node, E_RAFT_NODE_FLAG flag);

void raft_node_flag_add(raft_node_t* node, E_RAFT_NODE_FLAG flag);
void raft_node_flag_remove(raft_node_t* node, E_RAFT_NODE_FLAG flag);

u64 raft_node_flag_get(raft_node_t* node);
u64 raft_node_id_get(raft_node_t* node);

#endif

