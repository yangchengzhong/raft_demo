#include "raft_lib_callback.h"
#include "r_ctx.h"

static int  _raft_lib_send_cb(void* handle, raft_conn_t* peer_conn, ds_t* ds);
static void _raft_lib_on_become_follower(void* handle);
static void _raft_lib_on_become_candidate(void* handle);
static void _raft_lib_on_become_leader(void* handle);
static void _raft_lib_log(void* handle);

static raft_cb_t gs_raft_cb = 
{
    _raft_lib_send_cb,
    _raft_lib_on_become_follower,
    _raft_lib_on_become_candidate,
    _raft_lib_on_become_leader,
    _raft_lib_log
};

const raft_cb_t* raft_lib_callback_get()
{
    return &gs_raft_cb;
}

/*********************** static functions ************************/
static void _udp_send_uv_cb(uv_udp_send_t* req, int32_t status) 
{
    if(NULL == req) return ;

    if(NULL != req->data) ds_release((ds_t*)req->data);

    R_FREE(req);

    return ;
}
static int _udp_send(uv_udp_t *udp_handle, raft_conn_t* peer_conn, ds_t* ds)
{
    uv_udp_send_t* send_req = R_MALLOC(sizeof(uv_udp_send_t));
    assert_retval(NULL != send_req, ERR_BAD_ALLOC);

    send_req->data = ds_retain(ds);
    uv_buf_t buf = uv_buf_init((char*)ds->data, (int)ds->data_len);

    struct sockaddr_in dst_addr;
    dst_addr.sin_family = AF_INET;
    dst_addr.sin_addr.s_addr = inet_addr(peer_conn->ip);
    dst_addr.sin_port = htons(peer_conn->port);

    int ret = uv_udp_send(send_req, udp_handle, &buf, 1, (const struct sockaddr*)&dst_addr, _udp_send_uv_cb);
    if(0 != ret)
    {
        ds_release(ds);
        return ret;
    }

    return 0;
}

static int  _raft_lib_send_cb(void* handle, raft_conn_t* peer_conn, ds_t* ds)
{
    r_context_t* ctx = (r_context_t*)handle;
    assert_retval(NULL != ctx, ERR_INVALID_ARG);

    int ret = ERR_INIT;

    switch(peer_conn->sock_type)
    {
        case E_SOCKET_UDP:
            ret = _udp_send(&ctx->listener_ctx.listeners[E_SOCKET_UDP].listener_un.udp_listener.udp_handle, peer_conn, ds);
            break;
        default:
            assert_retval(0, ERR_INTERNAL);
            ret = ERR_INTERNAL;
            break;
    }
        
    return 0;
}

static void _raft_lib_on_become_follower(void* handle)
{


}

static void _raft_lib_on_become_candidate(void* handle)
{


}

static void _raft_lib_on_become_leader(void* handle)
{

}

static void _raft_lib_log(void* handle)
{


}

