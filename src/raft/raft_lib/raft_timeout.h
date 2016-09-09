#ifndef __RAFT_TIMEOUT_H__
#define __RAFT_TIMEOUT_H__

#include "uv.h"

void raft_loop_timeout(uv_timer_t* handle);

#endif

