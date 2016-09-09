#ifndef __RAFT_FSM_DEF_H__
#define __RAFT_FSM_DEF_H__

#include "raft_peer.h"
#include "d_str.h"

typedef enum 
{
    E_RAFT_FSM_STATE_FOLLOWER,
    E_RAFT_FSM_STATE_CANDIDATE,
    E_RAFT_FSM_STATE_LEADER,
    E_RAFT_FSM_STATE_MAX,
}E_RAFT_FSM_STATE;

typedef enum 
{
    E_RAFT_FSM_MSG_ELECTION_TIMEOUT,
    E_RAFT_FSM_MSG_VOTE_REQ,
    E_RAFT_FSM_MSG_VOTE_RSP,
    E_RAFT_FSM_MSG_ON_ENTRY,
    E_RAFT_FSM_MSG_RECV_ENTRY,
    E_RAFT_FSM_MSG_RECV_ENTRY_RSP,
    E_RAFT_FSM_MSG_APPENDENTRY_TIMEOUT,
}E_RAFT_FSM_MSG;
typedef struct
{
    raft_peer_t* peer;
    u64 reqer_id;
    u64 reqer_term;
    u64 last_entry_term;
    u64 last_entry_idx;
}raft_fsm_cb_vote_req_t;
typedef struct
{
    raft_peer_t* peer;
    int ret;
    u64 target_id;
    u64 target_term;
}raft_fsm_cb_vote_rsp_t;
typedef struct
{
    raft_peer_t* peer;
    u64 leader_id;
    u64 leader_term;
    u64 leader_committed_index;
    u64 prev_log_idx;
    u64 prev_log_term;
    int n_append_entry_arr;
    APPENDENTRY** append_entry_arr;
}raft_fsm_cb_entry_req_t;
typedef struct
{
    raft_peer_t* peer;
    int ret;
    u64 peer_term;
    u64 peer_match_index;
}raft_fsm_cb_entry_rsp_t;
typedef struct
{
    ds_t* entry_content;
}raft_fsm_cb_on_entry_t;

typedef struct
{
    u64 term;
    u64 candidate_id;
    u64 leader_id;
}raft_fsm_result_tofollower_t;
typedef struct
{  
    E_RAFT_FSM_MSG msg;
    union
    {
        raft_fsm_cb_vote_req_t  vote_req;
        raft_fsm_cb_vote_rsp_t  vote_rsp;
        raft_fsm_cb_entry_req_t entry_req;
        raft_fsm_cb_entry_rsp_t entry_rsp;
        raft_fsm_cb_on_entry_t  on_entry;
    }fsm_cb_un;
    union
    {
        raft_fsm_result_tofollower_t tofollower;
    }fsm_result_un;
}raft_fsm_cb_data_t;

#endif

