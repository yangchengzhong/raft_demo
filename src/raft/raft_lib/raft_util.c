#include "raft_util.h"
#include "util.h"

u64 raft_id_getter(const char* peer_ip, u16 port)
{
    return util_sockipport_2_u64(peer_ip, port);
}
