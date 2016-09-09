#ifndef __RAFT_ENTRY_H__
#define __RAFT_ENTRY_H__

#include "common_inc.h"
#include "d_str.h"

typedef struct
{
    u64     term;
    ds_t*   bin;
}raft_entry_t;

raft_entry_t* raft_entry_new(u64 term, ds_t* bin);
void raft_entry_delete(raft_entry_t* entry);

#endif

