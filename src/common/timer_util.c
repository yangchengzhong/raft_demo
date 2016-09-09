#include "timer_util.h"

#include "common_inc.h"

typedef struct
{
    void* cb_data;
    pfn_timer_cb_data_free cb_data_free_pfn;
}_uv_timer_cb_wrapper_t;

int _wrapper_timer_cb_init(uv_timer_t* timer, void* cb_data, pfn_timer_cb_data_free cb_data_free_pfn);
void _wrapper_timer_cb_fini(uv_timer_t* timer);

static uv_timer_cb gs_timer_cb[E_TIMER_TYPE_MAX] = {0};
int uv_timer_reg(uv_timer_cb timer_cb_map[], int count)
{  
    if(count > E_TIMER_TYPE_MAX) return ERR_NOTFOUND;

    for(int i = 0; i < count; ++i)
    {  
        gs_timer_cb[i] = timer_cb_map[i];   
    }

    return 0;
}

uv_timer_t* uv_add_timer(int start_interval
        , int repeat_interval
        , int enum_timer_type
        , void* cb_data
        , pfn_timer_cb_data_free cb_data_free_pfn)
{
    assert_retval(start_interval >= 0 && repeat_interval >= 0, NULL);

    if(enum_timer_type > E_TIMER_TYPE_MAX || NULL == gs_timer_cb[enum_timer_type]) return NULL;

    uv_timer_t* timer = malloc(sizeof(uv_timer_t));
    assert_retval(NULL != timer, NULL);

    int ret = uv_timer_init(uv_default_loop(), timer);
    if(0 != ret) 
    {
        free(timer);
        return NULL;
    }

    ret = _wrapper_timer_cb_init(timer, cb_data, cb_data_free_pfn);
    if(0 != ret)
    {
        uv_close((uv_handle_t*)timer, (uv_close_cb)free);
        return NULL;
    }

    ret = uv_timer_start(timer, gs_timer_cb[enum_timer_type], start_interval, repeat_interval);
    if(0 != ret) 
    {
        _wrapper_timer_cb_fini(timer);
        uv_close((uv_handle_t*)timer, (uv_close_cb)free);
        return NULL;
    }

    return timer;
}

int uv_del_timer(uv_timer_t* timer)
{
    assert_retval(NULL != timer, ERR_INVALID_ARG);

    _wrapper_timer_cb_fini(timer);

    uv_close((uv_handle_t*)timer, (uv_close_cb)free);

    return 0;
}

void* uv_get_timer_cb(uv_timer_t* timer)
{
    assert_retval(NULL != timer, NULL);

    return NULL == timer->data 
        ? NULL
        : ((_uv_timer_cb_wrapper_t*)timer->data)->cb_data;
}

/*********************** static functions ************************/
_uv_timer_cb_wrapper_t* _uv_timer_cb_wrapper_new(void* cb_data, pfn_timer_cb_data_free cb_data_free_pfn)
{
    assert_retval(NULL != cb_data, NULL);

    _uv_timer_cb_wrapper_t* timer_cb_wrapper = malloc(sizeof(_uv_timer_cb_wrapper_t));
    if(NULL == timer_cb_wrapper) return NULL;

    timer_cb_wrapper->cb_data = cb_data;
    timer_cb_wrapper->cb_data_free_pfn = cb_data_free_pfn;

    return timer_cb_wrapper;
}
void _uv_timer_cb_wrapper_delete(_uv_timer_cb_wrapper_t* timer_cb_wrapper)
{
    assert_retnone(NULL != timer_cb_wrapper);

    if(NULL != timer_cb_wrapper->cb_data && NULL != timer_cb_wrapper->cb_data_free_pfn)
    {
        timer_cb_wrapper->cb_data_free_pfn(timer_cb_wrapper->cb_data);
    }

    free(timer_cb_wrapper);

    return ;
}


int _wrapper_timer_cb_init(uv_timer_t* timer, void* cb_data, pfn_timer_cb_data_free cb_data_free_pfn)
{
    if(NULL == cb_data) 
    {
        timer->data = NULL;
        return 0;
    }

    timer->data = _uv_timer_cb_wrapper_new(cb_data, cb_data_free_pfn);
    if(NULL == timer->data) return ERR_BAD_ALLOC;

    return 0;
}
void _wrapper_timer_cb_fini(uv_timer_t* timer)
{
    assert_retnone(NULL != timer);

    if(NULL == timer->data) return ;

    _uv_timer_cb_wrapper_delete(timer->data);

    return ;
}

