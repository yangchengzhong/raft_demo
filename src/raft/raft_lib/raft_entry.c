#include "raft_entry.h"

raft_entry_t* raft_entry_new(u64 term, ds_t* bin)
{
    if(NULL == bin || INVALID64 == term) return NULL;

    raft_entry_t* e = malloc(sizeof(raft_entry_t));
    if(NULL == e) return NULL;

    e->term = term;
    e->bin = ds_retain(bin);

    return e;
}

void raft_entry_delete(raft_entry_t* entry)
{
    if(NULL == entry) return ;

    if(NULL != entry->bin) ds_release(entry->bin);

    return ;
}

