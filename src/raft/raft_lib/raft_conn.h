#ifndef __RAFT_CONN_H__
#define __RAFT_CONN_H__

#include "net.h"

typedef struct
{
    E_SOCKET_TYPE sock_type;
    const char* ip;
    u16 port;
    net_recv_ctx_t net_ctx;
}raft_conn_t;

int raft_conn_init(raft_conn_t *conn, E_SOCKET_TYPE sock_type, const char* peer_ip, u16 peer_port);

void raft_conn_fini(raft_conn_t *conn);

void raft_conn_set_netctx(raft_conn_t* conn, const net_recv_ctx_t* net_ctx);

#endif

