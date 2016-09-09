#include "raft_driver.h"

#include "raft_context.h"
#include "raft_recv.h"
#include "raft_fsm_msg_op.h"
#include "raft_fsm_meta.h"

int raft_driver_init(raft_context_t* ctx, void* cb_handler, const raft_cb_t* cb)
{
    assert_retval(NULL != ctx && NULL != cb_handler && NULL != cb, ERR_INVALID_ARG);

    /* init raft lib */
    int ret = raft_fsm_meta_init();
    if(0 != ret) return ret;

    raft_recv_init();

    ret = raft_ctx_init(ctx, cb_handler, cb);
    if(0 != ret) return ret;

    return 0;
}

u64 raft_driver_locate_leader_id(raft_context_t* ctx)
{
    assert_retval(NULL != ctx, INVALID64);

    return raft_ctx_get_leader_id(ctx);
}

int raft_driver_put(raft_context_t* ctx, ds_t* content)
{
    assert_retval(NULL != ctx && NULL != content, ERR_INVALID_ARG);

    return raft_fsm_msg_push_on_entry_msg(ctx, content);
}

