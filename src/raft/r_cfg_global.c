#include "r_cfg_global.h"

static r_global_cfg_t gs_global_cfg = {0};

r_global_cfg_t* r_cfg_get_global_cfg()
{
    return &gs_global_cfg;
}

raft_cfg_t* r_cfg_get_raft_cfg()
{
    return &gs_global_cfg.raft_cfg;
}

