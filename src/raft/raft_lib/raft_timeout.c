#include "raft_timeout.h"

#include "timer_util.h"
#include "raft_context.h"
#include "util.h"
#include "raft_fsm_msg_op.h"
#include "raft_op.h"

void raft_loop_timeout(uv_timer_t* handle)
{  
    assert_retnone(NULL != handle);

    raft_context_t* raft_ctx = uv_get_timer_cb(handle);
    assert_retnone(NULL != raft_ctx);

    u64 now = util_now();

    /* check for election timeout */
    if(now >= raft_ctx->election_timeout_timestamp)
    {
        raft_fsm_msg_push_election_timeout_msg(raft_ctx);
        raft_op_restart_election(raft_ctx);
    }

    /* check for append entry timeout */
    if(now >= raft_ctx->appendentry_timeout_timestamp)
    {
        raft_fsm_msg_push_append_entry_timeout_msg(raft_ctx);
        raft_op_restart_appendentry_timer(raft_ctx);
    }

    return ;
}

