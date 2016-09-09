#include "net.h"

#include <string.h>

static void _listen_recv_alloc_cb(uv_handle_t* handle, size_t suggested_size,uv_buf_t* buf);
static void _listen_udp_recv_cb(uv_udp_t* handle,
                               ssize_t nread,
                               const uv_buf_t* buf,
                               const struct sockaddr* addr,
                               unsigned flags);
static int net_recv_ctx_build_udp(uv_udp_t* handle, const struct sockaddr* addr, OUT net_recv_ctx_t* ctx);

int net_listener_init(net_listener_t* self
        , E_SOCKET_TYPE type
        , char* ip_str
        , int port
        , pfn_listern_recv recv_pfn)
{
    assert_retval(NULL != self 
            && type > E_SOCKET_NONE
            && type < E_SOCKET_MAX
            && NULL != ip_str
            && port > 0
            && port <= UINT16_MAX
            && NULL != recv_pfn
            , ERR_INVALID_ARG);

    self->type = type;
    self->recv_pfn = recv_pfn;
    
    struct sockaddr_in addr;
    int ret = uv_ip4_addr(ip_str, port, &addr);
    assert_retval(0 == ret, ret);

    switch(type)
    {
        case E_SOCKET_UDP:
        {

            net_listener_udp_t *listen_udp = &self->listener_un.udp_listener;
            listen_udp->udp_handle.data = self;

            ret = uv_udp_init(uv_default_loop(), &listen_udp->udp_handle);
            assert_retval(0 == ret, ret);

            ret = uv_udp_bind(&listen_udp->udp_handle
                    , (const struct sockaddr*) &addr
                    , 0);
            assert_retval(0 == ret, ret);

            ret = uv_udp_recv_start(&listen_udp->udp_handle, _listen_recv_alloc_cb, _listen_udp_recv_cb);

            LOGI("start listen udp ip[%s], port[%d]", ip_str, port);

            break;
        }

        case E_SOCKET_TCP:
        {
        
            break;
        }

        default:
            break;

    }

    return ret;
}

bool net_socket_type_valid(E_SOCKET_TYPE type)
{
    return type > E_SOCKET_NONE && type < E_SOCKET_MAX;
}

/*********************** static functions ************************/
static void _listen_recv_alloc_cb(uv_handle_t* handle, size_t suggested_size,uv_buf_t* buf)
{
    assert_retnone(NULL != handle && NULL != handle->data && NULL != buf);

    net_listener_t* listener = handle->data;
    buf->base = listener->recv_buf;
    buf->len = sizeof(listener->recv_buf);

    //assert_retnone(buf->len >= suggested_size);

    return ;
}

static void _listen_udp_recv_cb(uv_udp_t* handle,
                               ssize_t nread,
                               const uv_buf_t* buf,
                               const struct sockaddr* addr,
                               unsigned flags)
{
    if(NULL == addr || nread <= 0) return ;

    net_listener_t* listener = handle->data;

    net_recv_ctx_t ctx;
    int ret = net_recv_ctx_build_udp(handle, addr, &ctx);
    assert_retnone(0 == ret);

    listener->recv_pfn(&ctx, (const u8 *)buf->base, nread);

    return ;
}

/*********************** recv ctx ************************/
int net_recv_ctx_build_udp(uv_udp_t* handle, const struct sockaddr* addr, OUT net_recv_ctx_t* ctx)
{
    assert_retval(NULL != handle && NULL != addr && NULL != ctx, ERR_INVALID_ARG);

    ctx->sock_type = E_SOCKET_UDP;
    ctx->net_recv_ctx_un.udp_recv_ctx.addr = *addr;
    ctx->net_recv_ctx_un.udp_recv_ctx.handle = handle;

    return 0;
}

