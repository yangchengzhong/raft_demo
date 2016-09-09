#ifndef __R_CFG_GLOBAL_H__
#define __R_CFG_GLOBAL_H__

#include "r_def.h"
#include "net.h"
#include "raft_cfg.h"

typedef struct
{
    E_SOCKET_TYPE sock_type;
    char* ip_str;
    int port;
}r_net_elem_t;

typedef struct
{
    int listener_count;
    r_net_elem_t listeners[E_SOCKET_MAX];

    int peer_count;
    r_net_elem_t peer_nodes[R_GLOBAL_CFG_PEER_MAX];

    raft_cfg_t raft_cfg;

}r_global_cfg_t;

r_global_cfg_t* r_cfg_get_global_cfg();
raft_cfg_t* r_cfg_get_raft_cfg();


#endif

