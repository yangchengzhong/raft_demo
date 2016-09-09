#include <stdlib.h>
#include "vector.h"

vector_t* vector_new()
{
    vector_t* v = malloc(sizeof(vector_t));
    if(NULL == v) return NULL;

    if(NULL == (v->data = malloc(sizeof(void*)*VECTOR_INIT_CAPACITY)))
    {
        free(v);
        return NULL;
    }
    v->size = 0;
    v->capacity = 16;
    v->release_pfn = NULL;
    return v;
}

void vector_delete(vector_t *v)
{
    if(NULL == v) return ;
    
    if(NULL != v->release_pfn)
    {
        for(int i = 0; i < v->size; ++i)
        {
            if(NULL != v->data[i]) v->release_pfn(v->data[i]);
        }
    }

    free(v->data);
    v->size = 0;
    v->capacity = -1;
    free(v);

    return ;
}

void vector_set_release_handler(vector_t *v, func_vector_release_f pfn)
{
    if(NULL == v || NULL == pfn) return ;

    v->release_pfn = pfn;

    return ;
}

int vector_push_back(vector_t *v, void *elm)
{
    if(NULL == v || NULL == elm) return ERR_INVALID_ARGUMENT;

    if(v->size+1 > v->capacity)
    {
        if(NULL == (v->data = realloc(v->data,sizeof(void*) * v->capacity * 2)))
        {
            return ERR_INSUFFICIENT_RESOURCE;
        }
        v->capacity *= 2;
    }
    v->data[v->size++] = elm;
    return 0;
}

int vector_get_size(vector_t *v)
{
    if(!v)
    {
        return ERR_INVALID_ARGUMENT;
    }
    return v->size;
}
void *vector_get_head(vector_t *v)
{
    if(!v || v->size == 0)
    {
        return NULL;
    }
    return v->data[0];
}

void *vector_get(vector_t *v, int idx)
{
    if(!v || idx < 0)
    {
        return NULL;
    }
    if(idx >= v->size)
    {
        return NULL;
    }
    return v->data[idx];
}

void *vector_pop_back(vector_t *v)
{
    if(!v || v->size == 0)
    {
        return NULL;
    }
    v->size--;
    return v->data[v->size+1];
}

int vector_remove(vector_t *v, int idx)
{
    int i;
    if(!v || idx < 0 )
    {
        return ERR_INVALID_ARGUMENT;
    }
    if(idx >= v->size)
    {
        return ERR_INVALID_ARGUMENT;
    }

    if(NULL != v->data[idx] && NULL != v->release_pfn) v->release_pfn(v->data[idx]);

    v->size--;

    for(i=idx; i<v->size; i++)
    {
        v->data[i] = v->data[i+1];
    }
    return 0;
}

void *vector_pop_head(vector_t *v)
{
    void *rv;
    if(!v || v->size == 0)
    {
        return NULL;
    }
    rv = vector_get_head(v);
    vector_remove(v,0);
    return rv;
}

