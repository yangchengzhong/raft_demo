#ifndef __RAFT_PEER_H__
#define __RAFT_PEER_H__

#include "raft_conn.h"
#include "raft_node.h"

typedef struct
{
    raft_conn_t conn;/* conn info */
    raft_node_t node;

    u64         next_index;//next entry to send to the peer 
    u64         match_index;//already replicated index
}raft_peer_t;

raft_peer_t* raft_peer_new(E_SOCKET_TYPE sock_type, const char* peer_ip, u16 peer_port);
void raft_peer_delete(raft_peer_t* peer);

u64 raft_peer_get_next_index(const raft_peer_t* peer);
void raft_peer_set_next_index(raft_peer_t* peer, u64 next_index);

u64 raft_peer_get_match_index(const raft_peer_t* peer);
void raft_peer_set_match_index(raft_peer_t* peer, u64 match_index);

#endif

