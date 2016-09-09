#include "state_machine_def.h"
#include "state_machine.h"
#include "timer_util.h"

typedef struct
{
    int                     state_machine_type;
    state_machine_def_t*    sm_def;
}state_machine_index_t;

typedef struct
{
    int sm_type;
    u64 sm_holder_id;
}_sm_timer_cb_t;

state_machine_index_t g_state_machine_index[STATE_MACHINE_MAX_TYPE] = {0};

static inline bool _state_machine_type_valid(int type);
static void _change_state(state_machine_def_t *state_machine_def, void* sm_holder, state_machine_t *sm, void *msg_data, int msg_data_size, int next_state);
static int _sm_msg_dealer(state_machine_def_t *state_machine_def, state_machine_t *sm, int msg, void *msg_data, int msg_data_size);

int state_machine_def_init(state_machine_def_t *def)
{
    if(NULL == def) return ERR_SM_DEF_INIT;
    
    /* check sm_type valid */
    int sm_type = def->state_machine_type;
    if(!_state_machine_type_valid(sm_type)) return ERR_SM_DEF_INIT;

    /* check def content valid */
    if(NULL == def->get_sm_holder_by_sm_inst) return ERR_SM_DEF_INIT;
    if(def->entry_num >= STATE_MACHINE_MAX_ENTRY) return ERR_SM_DEF_INIT;

    bool need_timeout_def = false;
    for(int i = 0; i < def->entry_num; ++i)
    {
        if(def->entries[i].state != i || NULL == def->entries[i].sm_msg_dealer) return ERR_SM_DEF_INIT;

        if(def->entries[i].repeat_interval > 0) need_timeout_def = true;
    }
    
    if(need_timeout_def && (NULL == def->get_sm_holder_id || NULL == def->get_sm_inst_by_holder_id)) return ERR_SM_DEF_INIT;

    /* do init */
    g_state_machine_index[sm_type].state_machine_type = sm_type;
    g_state_machine_index[sm_type].sm_def = def;

    return 0;
}

int state_machine_msg_dealer(state_machine_def_t *state_machine_def, state_machine_t *sm, int msg, void *msg_data, int msg_data_size)
{
    return _sm_msg_dealer(state_machine_def, sm, msg, msg_data, msg_data_size);
}

int state_machine_inst_init(state_machine_def_t *state_machine_def, state_machine_t *sm, int init_state, void *msg_data, int msg_data_size)
{
    assert_retval(NULL != state_machine_def && NULL != sm && init_state < state_machine_def->entry_num, ERR_INVALID_ARG);
    
    sm->timeout_timer = NULL;
    sm->curr_state = init_state;

    state_machine_entry_def_t* entry = &state_machine_def->entries[init_state];
    if(NULL != entry->enter_action) 
    {
        void *sm_holder = state_machine_def->get_sm_holder_by_sm_inst(sm);
        entry->enter_action(sm_holder, msg_data, msg_data_size);
    }

    return 0;
}

int state_machin_inst_get_state(const state_machine_t *sm)
{
    assert_retval(NULL != sm, ERR_INVALID_ARG);

    return sm->curr_state;
}

void state_machine_timeout(uv_timer_t* handle)
{
    LOGD("state_machine_timeout");

    /* prepare to push msg to sm */
    _sm_timer_cb_t* timer_cb = uv_get_timer_cb(handle);
    assert_retnone(NULL != timer_cb);
    
    int type = timer_cb->sm_type;
    assert_retnone(_state_machine_type_valid(type));

    state_machine_index_t *index = &g_state_machine_index[type];
    assert_retnone(index->state_machine_type = type && NULL != index->sm_def);

    state_machine_def_t *state_machine_def = index->sm_def;
    assert_retnone(NULL != state_machine_def);

    state_machine_t *sm = state_machine_def->get_sm_inst_by_holder_id(timer_cb->sm_holder_id);
    if(NULL == sm)
    {
        LOGI("sm inst not found");
        return;
    }

    LOGD("state machine timeout, id<%lu> type<%d>", sm, type);

    /* push msg */
    int ret = _sm_msg_dealer(state_machine_def, sm, E_SM_PUBLIC_MSG_TIMEOUT, &timer_cb->sm_holder_id, sizeof(timer_cb->sm_holder_id));
    assert_retnone(0 == ret);

    return ;
}

/*********************** static functions ************************/
static void _leave_state(state_machine_def_t *state_machine_def
        , state_machine_t *sm
        , void* sm_holder
        , void *msg_data
        , int msg_data_size)
{
    /* base action */
    if(NULL != sm->timeout_timer)
    {
        int ret = uv_del_timer(sm->timeout_timer);
        assert_noeffect(0 == ret);

        sm->timeout_timer = NULL;
    }

    /* virtual action */
    state_machine_entry_def_t* entry = &state_machine_def->entries[sm->curr_state];
    if(NULL != entry->leave_action) entry->leave_action(sm_holder, msg_data, msg_data_size);

    return ;
}
static void _log_state_change(state_machine_def_t *state_machine_def
        , state_machine_t *sm
        , int next_state)
{
    state_machine_entry_def_t* cur_entry = &state_machine_def->entries[sm->curr_state];
    state_machine_entry_def_t* next_entry = &state_machine_def->entries[next_state];

    if(NULL != cur_entry->state_name && NULL != next_entry->state_name)
    {
        LOGD("state machine change state, id<%lu>, old<%s - %d> >> next<%s - %d>"
                   , sm
                   , cur_entry->state_name
                   , sm->curr_state
                   , next_entry->state_name
                   , next_state);
    }

    return ;
}
static _sm_timer_cb_t* _sm_timer_cb_new(int sm_type, u64 sm_holder_id)
{
    _sm_timer_cb_t* cb = malloc(sizeof(_sm_timer_cb_t));
    assert_retval(NULL != cb, NULL);

    cb->sm_type = sm_type;
    cb->sm_holder_id = sm_holder_id;

    return cb;
}
static void _sm_timer_cb_delete(_sm_timer_cb_t* sm_timer_cb)
{
    free(sm_timer_cb);

    return ;
}
static void _enter_state(state_machine_def_t *state_machine_def
        , state_machine_t *sm
        , void* sm_holder
        , int next_state
        , void *msg_data
        , int msg_data_size)
{
    /* set state */
    sm->curr_state = next_state;

    /* check if need timer */
    state_machine_entry_def_t* next_entry = &state_machine_def->entries[next_state];
    do
    {
        if(next_entry->repeat_interval <= 0)
        {
            sm->timeout_timer = NULL;
            break;
        }

        /* start for timer */
        u64 sm_holder_id = 0;
        int ret = state_machine_def->get_sm_holder_id(sm_holder, &sm_holder_id);
        assert_retnone(0 == ret);

        _sm_timer_cb_t* sm_timer_cb = _sm_timer_cb_new(state_machine_def->state_machine_type, sm_holder_id);
        assert_retnone(NULL != sm_timer_cb);

        sm->timeout_timer = uv_add_timer(next_entry->start_interval
                , next_entry->repeat_interval
                , E_TIMER_FSM_TIMEOUT
                , sm_timer_cb
                , (pfn_timer_cb_data_free)_sm_timer_cb_delete);
        if(NULL == sm->timeout_timer)
        {
            _sm_timer_cb_delete(sm_timer_cb);
            break;
        }
    }while(0);

    /* virtual action */
    if(NULL != next_entry->enter_action) next_entry->enter_action(sm_holder, msg_data, msg_data_size);
            
    return ;
}
static void _change_state(state_machine_def_t *state_machine_def
        , void* sm_holder
        , state_machine_t *sm
        , void *msg_data
        , int msg_data_size
        , int next_state)
{
    assert_retnone(NULL != state_machine_def 
            && NULL != sm_holder 
            && NULL != sm
            && NULL != msg_data 
            && msg_data_size >= 0
            && next_state < state_machine_def->entry_num);
    
    _leave_state(state_machine_def, sm, sm_holder, msg_data, msg_data_size);

    _log_state_change(state_machine_def, sm, next_state);

    _enter_state(state_machine_def, sm, sm_holder, next_state, msg_data, msg_data_size);

    return ;
}

static int _sm_msg_dealer(state_machine_def_t *state_machine_def, state_machine_t *sm, int msg, void *msg_data, int msg_data_size)
{
    /* check arg & status */
    if(NULL == state_machine_def || NULL == sm) return ERR_INVALID_ARG;

    if(sm->curr_state >= state_machine_def->entry_num) return ERR_INVALID_ARG;
    
    /* check if need log */
    state_machine_entry_def_t* entry = &state_machine_def->entries[sm->curr_state];
    assert_retval(entry->state == sm->curr_state, ERR_INTERNAL);
    
    if(NULL != entry->state_name)
    {
        //LOGD("state machine msg dealer, id<%lu> msg<%lu>, curr_state<%s - %d>"
        //        , sm
        //        , msg
        //        , entry->state_name
        //        , sm->curr_state);
    }
    
    /* do business msg dealer */
    bool need_change_state = false;
    int next_state = sm->curr_state;

    void *sm_holder = state_machine_def->get_sm_holder_by_sm_inst(sm);
    if(NULL == sm_holder) return ERR_NOTFOUND;

    entry->sm_msg_dealer(sm_holder, msg, msg_data, msg_data_size, &need_change_state, &next_state);

    /* check if need change state */
    if(need_change_state)
    {
        if(next_state >= state_machine_def->entry_num) return ERR_SM_DEF_STATE_INVALID;

        _change_state(state_machine_def, sm_holder, sm, msg_data, msg_data_size, next_state);
    }

    return 0;
}

static inline bool _state_machine_type_valid(int type)
{
    return type >= 0 && type < STATE_MACHINE_MAX_TYPE;
}

