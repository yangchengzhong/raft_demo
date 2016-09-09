#ifndef __ERROR_CODE_H__
#define __ERROR_CODE_H__

enum ERROR_CODE
{
    ERROR_CODE_COMMON           = -1000,
    ERROR_CODE_ARGPARSE         = -2000,
    ERROR_CODE_RESMNG           = -3000,
    ERROR_CODE_STATE_MACHINE    = -4000,
    ERROR_CODE_RAFT             = -5000,
};

enum ERROR_CODE_COMMON
{
    ERR_INVALID_ARG     = ERROR_CODE_COMMON - 1,
    ERR_NOTENOUGH       = ERROR_CODE_COMMON - 2,
    ERR_FILE            = ERROR_CODE_COMMON - 3,
    ERR_BAD_ALLOC       = ERROR_CODE_COMMON - 4,
    ERR_NOTFOUND        = ERROR_CODE_COMMON - 5,
    ERR_SIGNAL_REG      = ERROR_CODE_COMMON - 6,
    ERR_INIT            = ERROR_CODE_COMMON - 8,
    ERR_INTERNAL        = ERROR_CODE_COMMON - 9,
};

enum ERROR_CODE_ARGPARSE
{
    ERR_INPUT_INVALID   = ERROR_CODE_ARGPARSE - 1,
};

enum ERROR_CODE_RES_MNG
{
    ERR_FILE_NOTFOUND   = ERROR_CODE_RESMNG - 1,
    ERR_FILE_RESLOAD    = ERROR_CODE_RESMNG - 2,
    ERR_FILE_RESPARSE   = ERROR_CODE_RESMNG - 3,
    ERR_FILE_RESCHECK   = ERROR_CODE_RESMNG - 4,
};

enum ERROR_CODE_STATE_MACHINE
{
    ERR_SM_DEF_INIT             = ERROR_CODE_STATE_MACHINE - 1,
    ERR_SM_DEF_STATE_INVALID    = ERROR_CODE_STATE_MACHINE - 2,
};

enum ERROR_CODE_RAFT
{
    ERR_RAFT_TERM_INVALID               = ERROR_CODE_RAFT - 1,
    ERR_RAFT_ALREADY_VOTE               = ERROR_CODE_RAFT - 2,
    ERR_RAFT_PKG_OOD                    = ERROR_CODE_RAFT - 3,
    ERR_RAFT_LEADER_REDIRECT            = ERROR_CODE_RAFT - 4,
    ERR_RAFT_PREV_LOG_NOTFOUND          = ERROR_CODE_RAFT - 5,
    ERR_RAFT_PREV_LOG_TERM_NOT_MATCH    = ERROR_CODE_RAFT - 6,
    ERR_RAFT_MATCH_INDEX_INVALID        = ERROR_CODE_RAFT - 7,
    ERR_RAFT_INDEX_INVALID              = ERROR_CODE_RAFT - 8,
};

#endif

