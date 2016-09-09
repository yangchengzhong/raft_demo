#ifndef __RAFT_CFG_H__
#define __RAFT_CFG_H__

typedef struct
{
    int loop_timeout_msec;
    int election_timeout_min_msec;
    int election_timeout_max_msec;
    int append_entry_timeout_msec;
}raft_cfg_t;

#endif

