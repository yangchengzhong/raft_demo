#include "raft_fsm_def.h"

#include "r_def.h"
#include "state_machine.h"
#include "state_machine_def.h"
#include "raft_fsm_dealer_follower.h"
#include "raft_fsm_dealer_candidate.h"
#include "raft_fsm_dealer_leader.h"
#include "raft_context.h"
#include "util.h"

static int _raft_fsm_get_sm_holder_id(void *state_machine, u64 *sm_holder_id);
static state_machine_t* _raft_get_sm_inst_by_sm_holder_id(u64 sm_holder_id);
static void* _raft_get_sm_holder_by_sm_inst(state_machine_t *base);

extern const state_machine_entry_def_t* raft_fsm_follower_def_ref;

state_machine_entry_def_t gs_raft_sm_entry_def[] =
{
    {
        E_RAFT_FSM_STATE_FOLLOWER,
        raft_fsm_follower_state_dealer,
        raft_fsm_follower_leave_action,
        raft_fsm_follower_enter_action,
        0,
        0,
        "E_RAFT_FSM_STATE_FOLLOWER",
    },
    {
        E_RAFT_FSM_STATE_CANDIDATE,
        raft_fsm_candidate_state_dealer,
        raft_fsm_candidate_leave_action,
        raft_fsm_candidate_enter_action,
        0,
        0,
        "E_RAFT_FSM_STATE_CANDIDATE",
    },
    {
        E_RAFT_FSM_STATE_LEADER,
        raft_fsm_leader_state_dealer,
        raft_fsm_leader_leave_action,
        raft_fsm_leader_enter_action,
        0,
        0,
        "E_RAFT_FSM_STATE_LEADER",
    },
};

state_machine_def_t gs_raft_sm_def = 
{
    E_R_FSM_TYPE_RAFT_INST,
    E_RAFT_FSM_STATE_MAX,
    gs_raft_sm_entry_def,
    _raft_fsm_get_sm_holder_id,
    _raft_get_sm_inst_by_sm_holder_id,
    _raft_get_sm_holder_by_sm_inst,
};

state_machine_def_t* raft_fsm_get_meta()
{
    return &gs_raft_sm_def;
}

int raft_fsm_meta_init()
{
    return state_machine_def_init(&gs_raft_sm_def);
}

/**************************** static functions ***********************/
static int _raft_fsm_get_sm_holder_id(void *state_machine, u64 *sm_holder_id)
{
    assert_retval(NULL != state_machine && NULL != sm_holder_id, ERR_INVALID_ARG);

    raft_context_t* raft_ctx = _raft_get_sm_holder_by_sm_inst(state_machine);
    assert_retval(NULL != raft_ctx, ERR_NOTFOUND);

    //obviously, this addr in our case won't change
    *sm_holder_id = (u64)raft_ctx;

    return 0;
}

static state_machine_t* _raft_get_sm_inst_by_sm_holder_id(u64 sm_holder_id)
{
    raft_context_t* raft_ctx = (raft_context_t*)sm_holder_id;
    assert_retval(NULL != raft_ctx, NULL);

    return &raft_ctx->sm_inst;
}

static void* _raft_get_sm_holder_by_sm_inst(state_machine_t *base)
{
    return container_of(base, raft_context_t, sm_inst);
}

