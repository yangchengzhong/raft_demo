#include "r_recv.h"

#include "raft_recv.h"

//currently all msg driven by raft proto
int r_recv(net_recv_ctx_t* ctx, const u8 *data, int data_len)
{
    return raft_recv(ctx, data, data_len);
}

