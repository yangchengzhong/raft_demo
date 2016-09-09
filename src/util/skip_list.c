#include "skip_list.h"
#include <stdlib.h>
#include <stdio.h>

static const int _MAX_LEVEL = 25; /* 320MB */

static skiplist_t* _skiplist_new(int max_level);
static int _rand_level();
static void _skiplist_node_free(skiplist_t* sk_list, skiplist_node_t *x);

skiplist_t* skiplist_new()
{
    return _skiplist_new(_MAX_LEVEL);
}

skiplist_t* skiplist_new_detail(int max_level)
{
    if(max_level <= 0) return NULL;

    return _skiplist_new(max_level);
}

void skiplist_set_discard(skiplist_t* sk_list, skiplist_discard_pfn val_pfn)
{
    if(NULL == sk_list) return ;

    sk_list->discard_pfn = val_pfn;

    return ;
}

//node at top must has all link for each level
int skiplist_put(skiplist_t* sk_list, uint64_t key, void* value)
{
    if(NULL == sk_list || UINT64_MAX == key) return -1;

    skiplist_node_t *update[_MAX_LEVEL+1];
    skiplist_node_t *x = sk_list->header;

    /* try find the key node and find the update node for each level */
    for (int i = sk_list->level; i >= 1; i--) 
    {
        //find a node which is just before of cur node
        //each node at level i, surely has next[i]
        while (x->next[i]->key < key)
            x = x->next[i];//when find a x,all the left nodes are passed
        update[i] = x;
    }
    x = x->next[1];//0 is unused

    /* found */
    if (key == x->key) return -2;

    /* try insert new */
    int level = _rand_level();// this level tends to be from bottom 
    
    /* check if need to expand level */
    if (level > sk_list->level) 
    {
        for(int i = sk_list->level+1; i <= level; ++i) 
        {
            update[i] = sk_list->header;
        }
        sk_list->level = level;
    }

    /* create insert node */
    x = (skiplist_node_t *)malloc(sizeof(skiplist_node_t));
    if(NULL == x) return -3;

    x->key = key;
    x->value = value;
    x->next = (skiplist_node_t **)malloc(sizeof(skiplist_node_t*) * (level + 1));
    if(NULL == x->next) 
    {
        free(x);
        return -4;
    }
    for(int i = 1; i <= level; ++i) 
    {
        //for update[i] is choosed behind key before
        //like  cur->next = pre->next
        //      pre->next = cur;
        x->next[i] = update[i]->next[i];
        update[i]->next[i] = x;//next[i]: if ptr to one bottom node, the node surely links cur level,else to the tail 
    }

    ++sk_list->size;

    return 0;
}

int skiplist_remove(skiplist_t* sk_list, uint64_t key)
{
    skiplist_node_t *update[_MAX_LEVEL + 1];
    skiplist_node_t *x = sk_list->header;
    for(int i = sk_list->level; i >= 1; --i) 
    {
        while (x->next[i]->key < key)
            x = x->next[i];
        update[i] = x;
    }

    x = x->next[1];//located at bottom

    if(x->key != key) return -1;

    for(int i = 1; i <= sk_list->level; ++i) 
    {
        if (update[i]->next[i] != x) break;//ignore links which not contain x 
        update[i]->next[i] = x->next[i];//pre->next = cur->next;
    }
    _skiplist_node_free(sk_list, x);
    --sk_list->size;

    while(sk_list->level > 1 && sk_list->header->next[sk_list->level] == sk_list->header)
        sk_list->level--;
    return 0;
}

int skiplist_destroy(skiplist_t* sk_list)
{
    if(NULL == sk_list) return -1;

    if(NULL != sk_list->header)
    {
        skiplist_node_t *current_node = sk_list->header->next[1];
        while(current_node != sk_list->header) 
        {
            skiplist_node_t *next_node = current_node->next[1];
            _skiplist_node_free(sk_list, current_node);
            current_node = next_node;
        }

        free(sk_list->header->next);
        free(sk_list->header);
    }

    free(sk_list);

    return 0;
}

void* skiplist_get_value(skiplist_t* sk_list, uint64_t key)
{
    skiplist_node_t *x = sk_list->header;
    for(int i = sk_list->level; i >= 1; i--) 
    {
        while (x->next[i]->key < key)
            x = x->next[i];
    }

    return x->next[1]->key == key ? x->next[1]->value : NULL;
}

void* skiplist_get_value_byoffset(skiplist_t* sk_list, int offset)
{
    if(NULL == sk_list || offset < 0 || offset > sk_list->size) return NULL;

    skiplist_node_t *x = sk_list->header;

    while (x && x->next[1] != sk_list->header && offset > 0) 
    {
        --offset;

        x = x->next[1];
    }

    return (NULL != x && NULL != x->next[1]) ? x->next[1]->value : NULL;
}

void* skiplist_head_value(skiplist_t* sk_list)
{
    if(sk_list->size <= 0) return NULL;

    skiplist_node_t *x = sk_list->header;
    if(NULL == x) return NULL;

    return x->next[1]->value;
}

void skiplist_clear(skiplist_t* sk_list)
{
    if(NULL == sk_list) return ;

    int max_count = sk_list->size;
    for(int i = 0; i < max_count; ++i)
    {
        int ret = skiplist_head_pop(sk_list);
        if(0 != ret) return ;
    }

    return ;
}


int skiplist_head_pop(skiplist_t* sk_list)
{
    if(NULL == sk_list || sk_list->size <= 0) return -1;

    skiplist_node_t *x = sk_list->header;
    if(NULL == x) return -1;

    skiplist_remove(sk_list, x->next[1]->key);

    return 0;
}

int skiplist_size(const skiplist_t* sk_list)
{
    return sk_list->size;
}

int skiplist_iterate(skiplist_t* sk_list, visit_pfn pfn, void* ctx)
{
    skiplist_node_t *x = sk_list->header;

    while (x && x->next[1] != sk_list->header) 
    {
        int ret = pfn(ctx, x->next[1]->value);
        if(SKIPLIST_ITER_BREAK == ret) break;

        x = x->next[1];
    }
    
    return 0;
}

int skiplist_iterate_offset(skiplist_t* sk_list, visit_pfn pfn, void* ctx, int beg, int end)
{
    if(NULL == sk_list 
            || NULL == pfn 
            || beg < 0 
            || beg >= end 
            || end > sk_list->size)
    {
        return -1;
    }

    skiplist_node_t *x = sk_list->header;

    int i = 0;
    while(x && x->next[1] != sk_list->header && i < beg)
    {
        x = x->next[1];
        ++i;
    }

    /* from start_pos to end */
    while (x && x->next[1] != sk_list->header && i < end) 
    {
        int ret = pfn(ctx, x->next[1]->value);
        if(SKIPLIST_ITER_BREAK == ret) return 0;

        x = x->next[1];
        ++i;
    }

    return 0;
}

int skiplist_iterate_random(skiplist_t* sk_list, visit_pfn pfn, void* ctx)
{
    int size = sk_list->size;
    if(size <= 0) return 0;

    skiplist_node_t *x = sk_list->header;

    int start_pos = rand() % size;

    int i = 0;
    /* move to start pos */
    while (x && x->next[1] != sk_list->header && i < start_pos) 
    {
        x = x->next[1];
        ++i;
    }

    /* from start_pos to end */
    while (x && x->next[1] != sk_list->header && i < size) 
    {
        int ret = pfn(ctx, x->next[1]->value);
        if(SKIPLIST_ITER_BREAK == ret) return 0;

        x = x->next[1];
        ++i;
    }

    /* from beg to start pos */
    i = 0;
    x = sk_list->header;
    while (x && x->next[1] != sk_list->header && i < start_pos) 
    {
        int ret = pfn(ctx, x->next[1]->value);
        if(SKIPLIST_ITER_BREAK == ret) return 0;

        x = x->next[1];
        ++i;
    }

    return 0;
}

//static void skiplist_dump(skiplist_t *sk_list)
//{
//    skiplist_node_t *x = sk_list->header;
//    while (x && x->next[1] != sk_list->header) {
//        printf("%lu[%d]->", x->next[1]->key, *((int*)x->next[1]->value));
//        x = x->next[1];
//    }
//    printf("NIL\n");
//
//    printf("sklist size[%d]\n", sk_list->size);
//
//    return ;
//}

int visit(void* ctx, void* iter)
{
    int* x = iter;
    printf("[%d]->", *x);

    return 0;
}

//int main()
//{
//    int arr[] = {3, 6, 9, 2, 11, 1, 4, 4141,213,31,231,3,13,1,3,13,1}, i;
//    skiplist_t *list = skiplist_new();
//
//    srand(time(NULL));
//
//    printf("Insert:--------------------\n");
//    for (i = 0; i < sizeof(arr)/sizeof(arr[0]); i++) {
//        skiplist_put(list, arr[i], &arr[i]);
//    }
//    skiplist_dump(list);
//
//    //printf("Search:--------------------\n");
//    //int keys[] = {1, 4, 7, 10, 11, 22, 23, 32, 14, 151, 111};
//
//    //for (i = 0; i < sizeof(keys)/sizeof(keys[0]); i++) {
//    //    int* x = skiplist_get_value(list, keys[i]);
//    //    if (x) {
//    //        printf("key = %d, value = %d\n", keys[i], *((int*)x));
//    //    } else {
//    //        printf("key = %d, not fuound\n", keys[i]);
//    //    }
//    //}
//
//    skiplist_remove(list, 1);
//    skiplist_remove(list, 3);
//    skiplist_remove(list, 11);
//    skiplist_remove(list, 4);
//
//    skiplist_dump(list);
//
//    printf("head value = %d\n", *((int*)skiplist_head_value(list)));
//
//    skiplist_head_pop(list);
//    skiplist_head_pop(list);
//    skiplist_head_pop(list);
//    
//    printf("head value = %d\n", *((int*)skiplist_head_value(list)));
//
//    skiplist_destroy(list);
//
//    return 0;
//}

/*********************** static functions ************************/
static skiplist_t* _skiplist_new(int max_level)
{
    skiplist_t* sk_list = (skiplist_t*)malloc(sizeof(skiplist_t));
    if(NULL == sk_list) return NULL;

    skiplist_node_t* node = (skiplist_node_t*)malloc(sizeof(skiplist_node_t));
    if(NULL == node)
    {
        free(sk_list);
        return NULL;
    }

    sk_list->header = node;
    node->key = UINT64_MAX;
    node->next = (skiplist_node_t**)malloc(sizeof(skiplist_node_t*) * (max_level + 1));
    if(NULL == node->next)
    {
        free(node);
        free(sk_list);
        return NULL;
    }

    for(int i = 0; i <= max_level; ++i)
    {
        node->next[i] = sk_list->header;
    }

    sk_list->max_level = max_level;
    sk_list->level = 1;
    sk_list->size = 0;
    sk_list->discard_pfn = NULL;

    return sk_list;
}

static int _rand_level()
{
    int level = 1;
    while (rand() < RAND_MAX/2 && level < _MAX_LEVEL)
        level++;
    return level;
}

static void _skiplist_node_free(skiplist_t* sk_list, skiplist_node_t *x)
{
    if(NULL == sk_list || NULL == x) return ;

    if(NULL != sk_list->discard_pfn) sk_list->discard_pfn(x->value);

    free(x->next);

    free(x);

    return ;
}

