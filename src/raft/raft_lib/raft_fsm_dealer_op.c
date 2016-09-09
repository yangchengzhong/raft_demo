#include "raft_fsm_dealer_op.h"

#include "util_math.h"

int raft_fsm_dealder_op_prepare_2_follower(raft_fsm_result_tofollower_t* result_2_follower
        , u64 term
        , u64 candidate_id
        , u64 leader_id
        , bool *need_change_state
        , int *next_state)
{
    assert_retval(NULL != result_2_follower
            && NULL != need_change_state
            && NULL != next_state
            , ERR_INVALID_ARG);

    result_2_follower->term = term;
    result_2_follower->candidate_id = candidate_id;
    result_2_follower->leader_id = leader_id;

    *need_change_state = true;
    *next_state = E_RAFT_FSM_STATE_FOLLOWER;

    return 0;
}

int raft_fsm_dealer_op_try_remove_by_prev(raft_context_t* r_ctx, int entry_count,  u64 prev_log_idx, u64 prev_log_term)
{
    assert_retval(NULL != r_ctx, ERR_INVALID_ARG);

    raft_entry_t* prev_entry = raft_ctx_get_entry(r_ctx, prev_log_idx);
    assert_retval(NULL != prev_entry, ERR_INVALID_ARG);

    //check if diff at first
    if(prev_entry->term != prev_log_term)
    {
        raft_ctx_entry_remove_from_idx(r_ctx, prev_log_idx);
        return ERR_RAFT_PREV_LOG_TERM_NOT_MATCH;
    }

    //when eventually consistent, check tail
    int leader_tail_idx = prev_log_idx + 1;
    int my_tail_idx = raft_ctx_get_entry_tail_idx(r_ctx);
    if(INVALID64 == my_tail_idx) return 0;

    if(0 == entry_count && my_tail_idx > leader_tail_idx)
    {
        raft_ctx_entry_remove_from_idx(r_ctx, leader_tail_idx);
    }

    return 0;
}

int raft_fsm_dealer_op_append_entries(raft_context_t* r_ctx
        , u64 prev_log_idx
        , int n_append_entry_arr
        , APPENDENTRY** append_entry_arr)
{
    assert_retval(NULL != r_ctx && n_append_entry_arr >= 0 && NULL != append_entry_arr, ERR_INVALID_ARG);

    /* get start pos */
    int i = 0;
    for(; i < n_append_entry_arr; ++i)
    {
        APPENDENTRY* proto_append_entry = append_entry_arr[i];
        int append_entry_idx = prev_log_idx + 1 + i;

        raft_entry_t* entry = raft_ctx_get_entry(r_ctx, append_entry_idx);
        if(NULL == entry) break;//null at cur pos, replicate here

        //if diff here, remove to tail
        if(entry->term != proto_append_entry->entry_term)
        {
            raft_ctx_entry_remove_from_idx(r_ctx, append_entry_idx);
            break;
        }
    }

    /* exec replicate */
    for(; i < n_append_entry_arr; ++i)
    {
        APPENDENTRY* proto_append_entry = append_entry_arr[i];

        ds_t* entry_content = ds_new(proto_append_entry->entry_data.data, proto_append_entry->entry_data.len);
        if(NULL == entry_content) return ERR_BAD_ALLOC;

        raft_entry_t* entry = raft_entry_new(raft_ctx_get_term(r_ctx), entry_content);
        if(NULL == entry)
        {
            ds_release(entry_content);
            return ERR_BAD_ALLOC;
        }

        int _ret = raft_ctx_put_entry(r_ctx, entry);

        ds_release(entry_content);
        if(0 != _ret) return _ret;
    }

    return 0;
}

void raft_fsm_dealer_op_update_commit_idx(raft_context_t* r_ctx, u64 leader_committed_index)
{
    assert_retnone(NULL != r_ctx);

    u64 log_idx = raft_ctx_get_entry_tail_idx(r_ctx);

    raft_ctx_set_committed_index(r_ctx, MIN(leader_committed_index, log_idx));

    return ;
}

