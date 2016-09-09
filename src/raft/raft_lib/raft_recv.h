#ifndef __RAFT_RECV_H__
#define __RAFT_RECV_H__

#include "net.h"

void raft_recv_init();

int raft_recv(net_recv_ctx_t* ctx, const u8 *data, int data_len);

#endif

