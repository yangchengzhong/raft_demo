#include "common_inc.h"

#include <string.h>
#include "arg_parse.h"
#include "r_ctx.h"
#include "r_cfg_global.h"
#include "r_res_mng.h"
#include "r_signal.h"
#include "r_recv.h"
#include "raft_driver.h"
#include "raft_lib_callback.h"
#include "r_timer.h"

static int _init(int argc, char* argv[], r_context_t* ctx);
static int _prepare(r_context_t* ctx);
static int _run(r_context_t* ctx);

int main(int argc, char* argv[])
{
    r_context_t* ctx = r_get_ctx();
    assert_retval(NULL != ctx, ERR_INVALID_ARG);

    int ret = _init(argc, argv, ctx);
    if(0 != ret) return ret;

    ret = _prepare(ctx);
    if(0 != ret) return ret;

    ret = _run(ctx);
    if(0 != ret) return ret;

    return 0;
}

/*********************** static functions ************************/
static int _net_init()
{
    r_global_cfg_t* cfg = r_cfg_get_global_cfg();
    assert_retval(NULL != cfg, ERR_INVALID_ARG);

    r_context_listener_t* listeners_ctx = r_ctx_get_listener();
    assert_retval(NULL != listeners_ctx, ERR_INVALID_ARG);
    
    assert_retval(cfg->listener_count <= E_SOCKET_MAX, ERR_INVALID_ARG);
    r_net_elem_t* elem_cfg = &cfg->listeners[0];
    net_listener_t* elem_ctx = &listeners_ctx->listeners[0];

    int ret = net_listener_init(elem_ctx
            , elem_cfg->sock_type
            , elem_cfg->ip_str
            , elem_cfg->port
            , r_recv);
    assert_continue(0 == ret);

    return 0;
}

static int _init(int argc, char* argv[], r_context_t* ctx)
{
    int ret = arg_parse_init(argc, argv);
    if(0 != ret) return ret;

    ret = r_res_mng_init();
    if(0 != ret) return ret;

    ret = r_signal_init();
    if(0 != ret) return ret;

    ret = r_ctx_init();
    if(0 != ret) return ret;

    ret = _net_init();
    if(0 != ret) return ret;

    if(0 != ret) return ret;
    ret = r_timer_init();

    return 0;
}

static int _prepare(r_context_t* ctx)
{
    raft_context_t* raft_ctx = r_ctx_get_raft();
    if(NULL == raft_ctx) return ERR_NOTFOUND;

    int ret = raft_driver_init(raft_ctx, ctx, raft_lib_callback_get());
    if(0 != ret) return ret;

    return 0;
}

static int _run(r_context_t* ctx)
{
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    return 0;
}

