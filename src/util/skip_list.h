#ifndef __SKIPLIST_H__
#define __SKIPLIST_H__

#include <stdint.h>

#define SKIPLIST_FOR_EACH(sklist, sk_node) \
    for(sk_node = sklist->header; sk_node && sk_node->next[1] != sklist->header; sk_node = sk_node->next[1])
typedef enum
{
    SKIPLIST_ITER_CONTINUE,
    SKIPLIST_ITER_BREAK,
}SKIPLIST_ITER_ERRCODE;

typedef struct skiplist_node_t
{
    uint64_t                    key;
    void*                       value;
    struct skiplist_node_t**    next;
}skiplist_node_t;

typedef int (*visit_pfn)(void* ctx, void* iter);
typedef void (*skiplist_discard_pfn)(void*);

typedef struct
{
    int                     max_level;
    int                     level;
    int                     size; 
    skiplist_node_t*        header;

    skiplist_discard_pfn    discard_pfn;
}skiplist_t;

skiplist_t* skiplist_new();
skiplist_t* skiplist_new_detail(int max_level);
void skiplist_set_discard(skiplist_t* sk_list, skiplist_discard_pfn val_pfn);


/*##### if the same key already put, put will fail with errcode */
int skiplist_put(skiplist_t* sk_list, uint64_t key, void* value);
int skiplist_remove(skiplist_t* sk_list, uint64_t key);
int skiplist_destroy(skiplist_t* sk_list);


int skiplist_iterate(skiplist_t* sk_list, visit_pfn pfn, void* ctx);
int skiplist_iterate_random(skiplist_t* sk_list, visit_pfn pfn, void* ctx);
int skiplist_iterate_offset(skiplist_t* sk_list, visit_pfn pfn, void* ctx, int beg, int end);

void* skiplist_get_value(skiplist_t* sk_list, uint64_t key);
void* skiplist_get_value_byoffset(skiplist_t* sk_list, int offset);
void* skiplist_head_value(skiplist_t* sk_list);
int skiplist_head_pop(skiplist_t* sk_list);
int skiplist_size(const skiplist_t* sk_list);

void skiplist_clear(skiplist_t* sk_list);

#endif

