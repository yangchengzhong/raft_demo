#ifndef __TIMER_UTIL_H_
#define __TIMER_UTIL_H_

#include "common_inc.h"

typedef enum
{  
    E_TIMER_RAFT_LOOP   = 0,
    E_TIMER_FSM_TIMEOUT,
    E_TIMER_TYPE_MAX,
}E_TIMER_TYPE;

int uv_timer_reg(uv_timer_cb timer_cb_map[], int count);

typedef void (*pfn_timer_cb_data_free)(void* cb_data);
uv_timer_t* uv_add_timer(int start_interval
        , int repeat_interval
        , int enum_timer_type
        , void* cb_data
        , pfn_timer_cb_data_free cb_data_free_pfn);
int uv_del_timer(uv_timer_t* timer);
void* uv_get_timer_cb(uv_timer_t* timer);

#endif

