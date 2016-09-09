#ifndef VECTOR_H
#define VECTOR_H

#ifndef VECTOR_INIT_CAPACITY
#define VECTOR_INIT_CAPACITY 10
#endif

#ifndef ERR_INVALID_ARGUMENT
#define ERR_INVALID_ARGUMENT        -1
#endif
#ifndef ERR_INSUFFICIENT_RESOURCE
#define ERR_INSUFFICIENT_RESOURCE   -2
#endif

typedef void (*func_vector_release_f)(void*);

typedef struct {
    void **data;
    int size;
    int capacity;
    func_vector_release_f release_pfn; 
}vector_t;

vector_t* vector_new();
void vector_delete(vector_t *v);

/* get */
int vector_get_size(vector_t *v);
void *vector_get_head(vector_t *v);
void *vector_get(vector_t *v, int idx);

/* set */
void vector_set_release_handler(vector_t *v, func_vector_release_f pfn);
int vector_push_back(vector_t *v, void *elm);
void *vector_pop_head(vector_t *v);
void *vector_pop_back(vector_t *v);
int vector_remove(vector_t *v, int idx);

#endif
