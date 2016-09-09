#ifndef __STATE_MACHINE_DEF_H__
#define __STATE_MACHINE_DEF_H__

#include "common_inc.h" 

#define STATE_MACHINE_MAX_ENTRY 100
#define STATE_MACHINE_MAX_TYPE 256
typedef enum 
{
    E_SM_PUBLIC_MSG_MIN       = 100000,
    E_SM_PUBLIC_MSG_TIMEOUT   = 100001,
}E_SM_PUBLIC_MSG_TYPE;


/*********************** state machine entry def ************************/
//注意，即使返回错误码，只要need_change_state置为true，依然会调用重置状态
//need_change_state默认为false
typedef int (*pfn_sm_msg_dealer)(void *state_machine, int msg, void *msg_data, int msg_data_size, bool *need_change_state, int *next_state);
typedef void (*pfn_sm_leave_action)(void *sm_holder, void *msg_data, int msg_data_size);
typedef void (*pfn_sm_enter_action)(void *sm_holder, void *msg_data, int msg_data_size);
typedef struct 
{
    int                 state;          //状态，state的值必须跟所在entry在数组中的位置相等,entries[i].state == i
    pfn_sm_msg_dealer   sm_msg_dealer;  //状态机消息处理函数
    pfn_sm_leave_action leave_action;
    pfn_sm_enter_action enter_action;

    int                 start_interval; //隔几秒开始       
    int                 repeat_interval;//每个几秒一次
    const char          *state_name;    //状态名字，可选，调试用
}state_machine_entry_def_t;


/*********************** state machine def  ************************/
typedef struct 
{
    int         curr_state;
    uv_timer_t  *timeout_timer;
}state_machine_t;

/*********************** state machine def def ************************/
typedef int                 (*pfn_get_sm_holder_id)(void *state_machine, u64 *sm_holder_id);
typedef state_machine_t*    (*pfn_get_sm_inst_by_sm_holder_id)(u64 sm_holder_id);
typedef void*               (*pfn_get_sm_holder_by_sm_inst)(state_machine_t *base);
typedef struct 
{
    int                             state_machine_type;         //全服务器唯一的一个类型值，标识此服务器类型，不能为0
    int                             entry_num;                  //state machine状态数量
    state_machine_entry_def_t       *entries;              //state machine状态定义

    pfn_get_sm_holder_id            get_sm_holder_id;
    pfn_get_sm_inst_by_sm_holder_id get_sm_inst_by_holder_id;
    pfn_get_sm_holder_by_sm_inst    get_sm_holder_by_sm_inst;

}state_machine_def_t;

#endif 

