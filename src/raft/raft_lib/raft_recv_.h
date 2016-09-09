#ifndef __RAFT_RECV__H__
#define __RAFT_RECV__H__

#include "raft_peer.h"

void r_recv_vote_req(raft_peer_t* peer, const RAFTPKG* pkg);

void r_recv_vote_rsp(raft_peer_t* peer, const RAFTPKG* pkg);

void r_recv_append_entries_req(raft_peer_t* peer, const RAFTPKG* pkg);

void r_recv_append_entries_rsp(raft_peer_t* peer, const RAFTPKG* pkg);

#endif

