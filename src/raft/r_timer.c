#include "r_timer.h"
#include "timer_util.h"
#include "raft_timeout.h"
#include "state_machine.h"

static uv_timer_cb gs_r_timer_cb_map[E_TIMER_TYPE_MAX] = { 0 };

int r_timer_init()
{  
    gs_r_timer_cb_map[E_TIMER_RAFT_LOOP]    = raft_loop_timeout;
    gs_r_timer_cb_map[E_TIMER_FSM_TIMEOUT]  = state_machine_timeout;

    return uv_timer_reg(gs_r_timer_cb_map, sizeof(gs_r_timer_cb_map)/sizeof(gs_r_timer_cb_map[0]));
}

