#include "raft_recv.h"

#include "raft_recv_.h"
#include "raft_context.h"
#include "r_ctx.h"

typedef void (*func_r_recv_handler_f)(raft_peer_t* peer, const RAFTPKG* pkg); 
static func_r_recv_handler_f gs_recv_handler_map[E__RAFT__MSG__MAX] = {0};

static void _recv_reg(ERAFTMSG pkg_type, func_r_recv_handler_f recv_pfn);

void raft_recv_init()
{
    _recv_reg(E__RAFT__MSG__VOTE_SELF_REQ,      r_recv_vote_req);
    _recv_reg(E__RAFT__MSG__VOTE_SELF_RSP,      r_recv_vote_rsp);
    _recv_reg(E__RAFT__MSG__APPEND_ENTRIES_REQ, r_recv_append_entries_req);
    _recv_reg(E__RAFT__MSG__APPEND_ENTRIES_RSP, r_recv_append_entries_rsp);

    return ;
}

int raft_recv(net_recv_ctx_t* ctx, const u8 *data, int data_len)
{
    RAFTPKG *pkg = raft__pkg__unpack(NULL, data_len, data);
    assert_retval(NULL != pkg, ERR_INVALID_ARG);

    do
    {
        ERAFTMSG pkg_type = pkg->msg_type;
        if(pkg_type >= E__RAFT__MSG__MAX) break; 

        if(NULL == gs_recv_handler_map[pkg_type]) break;

        //currently not relashional with mem-change
        raft_peer_t* peer = raft_ctx_find_peer(r_ctx_get_raft(), ctx);
        if(NULL == peer) break;
        raft_conn_set_netctx(&peer->conn, ctx);

        gs_recv_handler_map[pkg_type](peer, pkg);

    }while(0);

    raft__pkg__free_unpacked(pkg, NULL);

    return 0;
}

/*********************** static functions ************************/
static void _recv_reg(ERAFTMSG pkg_type, func_r_recv_handler_f recv_pfn)
{
    assert_retnone(pkg_type >= 0 
            && pkg_type < E__RAFT__MSG__MAX
            && NULL != recv_pfn
            && NULL == gs_recv_handler_map[pkg_type]);

    gs_recv_handler_map[pkg_type] = recv_pfn;

    return ;
}

