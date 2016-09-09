#include "raft_node.h"

int raft_node_init(raft_node_t *node, u64 id, u64 flags)
{
    assert_retval(NULL != node, ERR_INVALID_ARG);

    node->id = id;
    node->flags = flags;

    return 0;
}

void raft_node_fini(raft_node_t* node)
{
    assert_retnone(NULL != node);

    return ;
}

bool raft_node_flag_contains(raft_node_t* node, E_RAFT_NODE_FLAG flag)
{
    assert_retval(NULL != node, false);

    return node->flags & flag;
}

void raft_node_flag_add(raft_node_t* node, E_RAFT_NODE_FLAG flag)
{
    assert_retnone(NULL != node);

    node->flags |= flag;

    return ;
}

void raft_node_flag_remove(raft_node_t* node, E_RAFT_NODE_FLAG flag)
{
    assert_retnone(NULL != node);

    node->flags &= 0xffffffff ^ flag;

    return ;
}

u64 raft_node_flag_get(raft_node_t* node)
{
    assert_retval(NULL != node, INVALID64);

    return node->flags;
}

u64 raft_node_id_get(raft_node_t* node)
{
    assert_retval(NULL != node, INVALID64);

    return node->id;
}

