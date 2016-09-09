#include "raft_conn.h"

int raft_conn_init(raft_conn_t *conn, E_SOCKET_TYPE sock_type, const char* peer_ip, u16 peer_port)
{
    conn->sock_type = sock_type;
    //conn->ip = inet_addr(peer_ip);
    conn->ip = peer_ip;
    conn->port = peer_port;

    return 0;
}

void raft_conn_fini(raft_conn_t *conn)
{
    return ;
}

void raft_conn_set_netctx(raft_conn_t* conn, const net_recv_ctx_t* net_ctx)
{
    assert_retnone(NULL != conn);

    conn->net_ctx = *net_ctx;

    return;
}

