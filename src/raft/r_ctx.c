#include "r_ctx.h"

#include <string.h>

static r_context_t gs_ctx;

int r_ctx_init()
{
    memset(&gs_ctx, 0x0, sizeof(gs_ctx));

    return 0;
}

r_context_t* r_get_ctx()
{
    return &gs_ctx;
}

r_context_listener_t* r_ctx_get_listener()
{
    return &gs_ctx.listener_ctx;
}

raft_context_t* r_ctx_get_raft()
{
    return &gs_ctx.raft_ctx;
}

