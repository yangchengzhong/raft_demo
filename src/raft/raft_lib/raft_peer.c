#include "raft_peer.h"

#include "raft_util.h"

raft_peer_t* raft_peer_new(E_SOCKET_TYPE sock_type, const char* peer_ip, u16 peer_port)
{
    // check previous by cfg check
    assert_retval(NULL != peer_ip, NULL);

    raft_peer_t* peer = R_MALLOC(sizeof(raft_peer_t));
    if(NULL == peer) return NULL;

    peer->next_index = INVALID64;
    peer->match_index = INVALID64;

    /* conn info */
    int ret = raft_conn_init(&peer->conn, sock_type, peer_ip, peer_port);
    if(0 != ret)
    {
        R_FREE(peer);
        return NULL;
    }

    ret = raft_node_init(&peer->node, raft_id_getter(peer_ip, peer_port), E_RAFT_NODE_PATICIPATE_VOTING);
    if(0 != ret)
    {
        raft_conn_fini(&peer->conn);
        R_FREE(peer);
        return NULL;
    }

    return peer;
}

void raft_peer_delete(raft_peer_t* peer)
{
    if(NULL == peer) return ;

    raft_conn_fini(&peer->conn);

    R_FREE(peer);

    return ;
}

u64 raft_peer_get_next_index(const raft_peer_t* peer)
{
    assert_retval(NULL != peer, INVALID64);

    return peer->next_index;
}
void raft_peer_set_next_index(raft_peer_t* peer, u64 next_index)
{
    assert_retnone(NULL != peer);

    peer->next_index = next_index;

    return ;
}

u64 raft_peer_get_match_index(const raft_peer_t* peer)
{
    assert_retval(NULL != peer, INVALID64);

    return peer->match_index;
}
void raft_peer_set_match_index(raft_peer_t* peer, u64 match_index)
{
    assert_retnone(NULL != peer);

    peer->match_index = match_index;

    return ;
}

