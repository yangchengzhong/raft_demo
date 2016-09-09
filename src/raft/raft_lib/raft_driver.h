#ifndef __RAFT_DRIVER_H__
#define __RAFT_DRIVER_H__

#include "raft_context.h"
#include "raft_lib_callback.h"

/* init */
int raft_driver_init(raft_context_t* ctx, void* cb_handler, const raft_cb_t* cb);

/* get */
u64 raft_driver_locate_leader_id(raft_context_t* ctx);

/* set */
int raft_driver_put(raft_context_t* ctx, ds_t* content);

#endif

