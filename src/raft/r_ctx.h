#ifndef __R_CTX_H__
#define __R_CTX_H__

#include "net.h"
#include "r_def.h"
#include "raft_context.h"

typedef struct
{
    int listener_count;
    net_listener_t listeners[E_SOCKET_MAX];
}r_context_listener_t;

typedef struct
{
    r_context_listener_t    listener_ctx;
    raft_context_t          raft_ctx;
}r_context_t;

int r_ctx_init();

r_context_t* r_get_ctx();
r_context_listener_t* r_ctx_get_listener();
raft_context_t* r_ctx_get_raft();

#endif

