#include "raft_send.h"

#include "raft_const.h"

static int _send_pkg(raft_context_t* raft_ctx, raft_conn_t* conn, const RAFTPKG* pkg);

int raft_send_vote_req(raft_context_t* raft_ctx, raft_conn_t* conn, u64 self_id, int cur_term, u64 last_entry_term, u64 last_entry_idx)
{
    assert_retval(NULL != raft_ctx 
            && NULL != conn 
            && NULL != raft_ctx->biz_cb_handler
            && NULL != raft_ctx->biz_cb.send_cb
            , ERR_INVALID_ARG);

    LOGD("raft_send_vote_req, self_id[%llu], cur_term[%d], last_entry_term[%llu], last_entry_idx[%llu]"
            , self_id
            , cur_term
            , last_entry_term
            , last_entry_idx);

    RAFTPKG pkg = {0};
    raft__pkg__init(&pkg);

    pkg.msg_type = E__RAFT__MSG__VOTE_SELF_REQ;
    
    PKGVOTESELFREQ vote_req;
    pkg__vote__self__req__init(&vote_req);
    pkg.vote_self_req = &vote_req;

    vote_req.reqer_id = self_id;
    vote_req.reqer_term = cur_term;
    vote_req.last_entry_term = last_entry_term;
    vote_req.last_entry_idx = last_entry_idx;

    return _send_pkg(raft_ctx, conn, &pkg);
}

int raft_send_vote_rsp(raft_context_t* raft_ctx, raft_conn_t* conn, u64 self_id, u64 cur_term, int ret)
{
    assert_retval(NULL != raft_ctx 
            && NULL != conn 
            && NULL != raft_ctx->biz_cb_handler
            && NULL != raft_ctx->biz_cb.send_cb
            , ERR_INVALID_ARG);

    LOGD("raft_send_vote_rsp, cur_term[%llu], ret[%d]", cur_term, ret);

    RAFTPKG pkg = {0};
    raft__pkg__init(&pkg);

    pkg.msg_type = E__RAFT__MSG__VOTE_SELF_RSP;
    
    PKGVOTESELFRSP vote_rsp;
    pkg__vote__self__rsp__init(&vote_rsp);
    pkg.vote_self_rsp = &vote_rsp;

    vote_rsp.ret = ret;
    vote_rsp.target_id = self_id;
    vote_rsp.target_term = cur_term;

    return _send_pkg(raft_ctx, conn, &pkg);
}

int raft_send_append_entries(raft_context_t* raft_ctx, raft_peer_t* peer, raft_conn_t* conn)
{
    assert_retval(NULL != raft_ctx 
            && NULL != peer
            && NULL != conn
            && NULL != raft_ctx->biz_cb_handler
            && NULL != raft_ctx->biz_cb.send_cb
            , ERR_INVALID_ARG);

    LOGD("raft_send_append_entries");

    RAFTPKG pkg = {0};
    raft__pkg__init(&pkg);

    pkg.msg_type = E__RAFT__MSG__APPEND_ENTRIES_REQ;
    
    PKGAPPENDENTRIESREQ append_entry_req;
    pkg__append__entries__req__init(&append_entry_req);
    pkg.append_entries_req = &append_entry_req;

    append_entry_req.leader_id = raft_ctx_get_id(raft_ctx);
    append_entry_req.leader_term = raft_ctx_get_term(raft_ctx);
    append_entry_req.leader_committed_index = raft_ctx_get_committed_index(raft_ctx);

    //currently, only send one log
    APPENDENTRY ae[RAFT_APPEND_ENTRY_SEND_ONCE_MAX];

    u64 next_index = raft_peer_get_next_index(peer);
    raft_entry_t* log_entry = raft_ctx_get_entry(raft_ctx, next_index);
    if(NULL != log_entry)
    {
        append_entry_req.n_append_entry_arr = 1;
        append_entry_req.append_entry_arr = (APPENDENTRY**)&ae;
        append_entry_req.append_entry_arr[0]->entry_term = log_entry->term;
        append_entry_req.append_entry_arr[0]->entry_data.data = log_entry->bin->data;
        append_entry_req.append_entry_arr[0]->entry_data.len = log_entry->bin->data_len;
        //next won't be invalid64
        append_entry_req.prev_log_idx = next_index > 0 ? next_index - 1 : INVALID64; 
    }
    else
    {
        append_entry_req.n_append_entry_arr = 0;
        append_entry_req.prev_log_idx = INVALID64;
    }

    return _send_pkg(raft_ctx, conn, &pkg);
}

int raft_send_append_entry_rsp(raft_context_t* raft_ctx, raft_conn_t* conn, int ret, u64 cur_term, u64 match_index)
{
    assert_retval(NULL != raft_ctx 
            && NULL != conn 
            && NULL != raft_ctx->biz_cb_handler
            && NULL != raft_ctx->biz_cb.send_cb
            , ERR_INVALID_ARG);

    LOGD("raft_send_append_entry_rsp, cur_term[%llu], match_index[%d], ret[%d]", cur_term, match_index, ret);

    RAFTPKG pkg = {0};
    raft__pkg__init(&pkg);

    pkg.msg_type = E__RAFT__MSG__APPEND_ENTRIES_RSP;
    
    PKGAPPENDENTRIESRSP append_entry_rsp;
    pkg__append__entries__rsp__init(&append_entry_rsp);
    pkg.append_entries_rsp = &append_entry_rsp;

    append_entry_rsp.ret = ret;
    append_entry_rsp.peer_term = cur_term;
    append_entry_rsp.peer_match_index = match_index;

    return _send_pkg(raft_ctx, conn, &pkg);
}

/*********************** static functions ************************/
static int _send_pkg(raft_context_t* raft_ctx, raft_conn_t* conn, const RAFTPKG* pkg)
{
    size_t size = raft__pkg__get_packed_size(pkg);
    ds_t *ds = ds_new_len(size);
    assert_retval(NULL != ds, ERR_INVALID_ARG);

    raft__pkg__pack(pkg, ds->data);
    ds->data_len = size;

    int ret = raft_ctx->biz_cb.send_cb(raft_ctx->biz_cb_handler, conn, ds_retain(ds));

    ds_release(ds);

    return ret;
}

