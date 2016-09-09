#ifndef __raft_callback_h__
#define __raft_callback_h__

#include "common_inc.h"
#include "d_str.h"
#include "raft_conn.h"

#define RAFT_CB_VALID(handler) (NULL != handler)

typedef int (*func_raft_send_cb_f)(void* handle, raft_conn_t* peer_conn, ds_t* ds);
typedef void (*func_raft_on_become_follower_f)(void* handle);
typedef void (*func_raft_on_become_candidate_f)(void* handle);
typedef void (*func_raft_on_become_leader_f)(void* handle);
typedef void (*func_raft_log_f)(void* handle);

typedef struct
{  
    func_raft_send_cb_f                 send_cb;
    func_raft_on_become_follower_f      on_follower_cb;
    func_raft_on_become_candidate_f     on_candidate_cb;
    func_raft_on_become_leader_f        on_leader_cb;
    
    func_raft_log_f                     log_cb;
}raft_cb_t;


#endif

