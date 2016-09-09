#ifndef __NET_H__
#define __NET_H__

#include "common_inc.h"

#define NET_LISTENER_RECV_BUF_MAX 10240
typedef enum
{
    E_SOCKET_NONE = -1,
    E_SOCKET_UDP,
    E_SOCKET_TCP,
    E_SOCKET_MAX,
}E_SOCKET_TYPE;
typedef struct
{
    uv_udp_t* handle;
    struct sockaddr addr;
}net_recv_ctx_udp_t;
typedef struct
{
    E_SOCKET_TYPE sock_type;
    union
    {
        net_recv_ctx_udp_t udp_recv_ctx;
    }net_recv_ctx_un;
}net_recv_ctx_t;

typedef struct
{
    uv_udp_t udp_handle;
}net_listener_udp_t;
typedef struct
{
    uv_tcp_t tcp_handle;
}net_listener_tcp_t;
typedef int (*pfn_listern_recv)(net_recv_ctx_t* ctx, const u8 *data, int data_len);
typedef struct
{
    E_SOCKET_TYPE type;
    char recv_buf[NET_LISTENER_RECV_BUF_MAX];
    pfn_listern_recv recv_pfn;

    union
    {
        net_listener_udp_t udp_listener;
        net_listener_tcp_t tcp_listener;
    }listener_un;
}net_listener_t;
int net_listener_init(net_listener_t* self
        , E_SOCKET_TYPE type
        , char* ip_str
        , int port
        , pfn_listern_recv recv_pfn);

bool net_socket_type_valid(E_SOCKET_TYPE type);

#endif

