#include "r_res_mng.h"

#include <string.h>
#include "res_mng.h"
#include "arg_parse.h"
#include "r_cfg_global.h"
#include "util.h"

static int _global_res_parser(config_t *cfg);
static int _global_res_checker();

int r_res_mng_init()
{
    RES_MNG_UNIT_DEF gs_res_unit_map[] =
    {
        {
            R_RES_ID_GLOBAL_CFG,
            {0},
            arg_parse_get_global_cfg_name,
            _global_res_parser,
            _global_res_checker,
            NULL,
        }
    };
    
    return res_mng_init(gs_res_unit_map
            , sizeof(gs_res_unit_map)/sizeof(gs_res_unit_map[0]));
}

/*********************** static functions ************************/
static inline E_SOCKET_TYPE _convert_listener_type(const char* proto_type)
{
    assert_retval(NULL != proto_type, E_SOCKET_NONE);

    if(0 == strcmp(proto_type, "tcp")) return E_SOCKET_TCP;
    if(0 == strcmp(proto_type, "udp")) return E_SOCKET_UDP;

    return E_SOCKET_NONE;
}
static int _parse_net_elem(int cfg_count, config_setting_t* cfg, r_net_elem_t net_elem_arr[])
{
    for(int i = 0; i < cfg_count; ++i)
    {
        const config_setting_t* listener_res = config_setting_get_elem(cfg, i);
        assert_retval(NULL != listener_res, ERR_FILE_RESPARSE);

        const char* proto_ptr = NULL;
        int ret = config_setting_lookup_string(listener_res, "type", (const char**)&proto_ptr);
        assert_retval(CONFIG_TRUE == ret, ERR_FILE_RESPARSE);

        net_elem_arr[i].sock_type = _convert_listener_type(proto_ptr);
        assert_retval(E_SOCKET_NONE != net_elem_arr[i].sock_type, ERR_FILE_RESPARSE);

        ret = config_setting_lookup_string(listener_res, "ip", (const char**)&net_elem_arr[i].ip_str);
        assert_retval(CONFIG_TRUE == ret, ERR_FILE_RESPARSE);

        ret = config_setting_lookup_int(listener_res, "port", &net_elem_arr[i].port);
        assert_retval(CONFIG_TRUE == ret, ERR_FILE_RESPARSE);
    }

    return 0;

}

static int _parse_listeners(config_t *cfg)
{
    config_setting_t* listeners_res_arr = config_lookup(cfg, "global.listeners");
    assert_retval(NULL != listeners_res_arr
            && config_setting_is_list(listeners_res_arr)
            && config_setting_length(listeners_res_arr) >= 1
            , ERR_FILE_RESPARSE);

    r_global_cfg_t* global_cfg = r_cfg_get_global_cfg();
    assert_retval(NULL != global_cfg, ERR_INVALID_ARG);

    global_cfg->listener_count = config_setting_length(listeners_res_arr);
    if(global_cfg->listener_count > E_SOCKET_MAX)
    {
        LOGE("listeners[%d] overmax[%d]", global_cfg->listener_count, E_SOCKET_MAX);
        return ERR_FILE_RESPARSE;
    }

    int ret = _parse_net_elem(global_cfg->listener_count, listeners_res_arr, global_cfg->listeners);
    if(0 != ret) return ret;

    return 0;
}

static int _parse_peer_nodes(config_t *cfg)
{
    config_setting_t* nodes_res = config_lookup(cfg, "global.peer_nodes");
    assert_retval(NULL != nodes_res
            && config_setting_is_list(nodes_res)
            && config_setting_length(nodes_res) >= 1
            , ERR_FILE_RESPARSE);

    r_global_cfg_t* global_cfg = r_cfg_get_global_cfg();
    assert_retval(NULL != global_cfg, ERR_INVALID_ARG);

    global_cfg->peer_count = config_setting_length(nodes_res);
    if(global_cfg->peer_count > R_GLOBAL_CFG_PEER_MAX)
    {
        LOGE("peer nodes[%d] overmax[%d]", global_cfg->peer_count, R_GLOBAL_CFG_PEER_MAX);
        return ERR_FILE_RESPARSE;
    }

    int ret = _parse_net_elem(global_cfg->peer_count, nodes_res, global_cfg->peer_nodes);
    if(0 != ret) return ret;

    return 0;
}

static int _parse_raft(config_t *cfg)
{
    raft_cfg_t* raft_cfg = r_cfg_get_raft_cfg();
    assert_retval(NULL != raft_cfg, ERR_INVALID_ARG);

    config_setting_t *global = config_lookup(cfg, "global");
    if(NULL == global)
    {
        LOGE("fail to find global cfg");
        return ERR_NOTFOUND;
    }

    int ret = config_setting_lookup_int(global
            , "election_timeout_min_msec"
            , &raft_cfg->election_timeout_min_msec);
    if(CONFIG_TRUE != ret) return ERR_FILE_RESPARSE;

    ret = config_setting_lookup_int(global
            , "election_timeout_max_msec"
            , &raft_cfg->election_timeout_max_msec);
    if(CONFIG_TRUE != ret) return ERR_FILE_RESPARSE;

    ret = config_setting_lookup_int(global
            , "loop_timeout_msec"
            , &raft_cfg->loop_timeout_msec);
    if(CONFIG_TRUE != ret) return ERR_FILE_RESPARSE;

    ret = config_setting_lookup_int(global
            , "append_entry_timeout_msec"
            , &raft_cfg->append_entry_timeout_msec);
    if(CONFIG_TRUE != ret) return ERR_FILE_RESPARSE;

    return 0;
}

int _global_res_parser(config_t *cfg)
{
    assert_retval(NULL != cfg, ERR_INVALID_ARG);

    int ret = _parse_listeners(cfg);
    if(0 != ret) return ERR_FILE_RESPARSE;

    ret = _parse_peer_nodes(cfg);
    if(0 != ret) return ERR_FILE_RESPARSE;

    ret = _parse_raft(cfg);
    if(0 != ret) return ERR_FILE_RESPARSE;

    return 0;
}

int _global_res_checker()
{
    r_global_cfg_t* global_cfg = r_cfg_get_global_cfg();
    assert_retval(NULL != global_cfg, ERR_INVALID_ARG);

    int ret = 0;

    /* check for listeners */
    if(global_cfg->listener_count <= 0)
    {
        LOGE("global cfg listener count invalid[%d]", global_cfg->listener_count);
        ret = ERR_FILE_RESPARSE;
    }
    for(int i = 0; i < global_cfg->listener_count; ++i)
    {
        r_net_elem_t *elem = &global_cfg->listeners[i];       

        if(!net_socket_type_valid(elem->sock_type) 
                || !util_is_ip_valid(elem->ip_str)
                || !util_is_port_valid(elem->port))
        {
            LOGE("global cfg listener[%d] invalid", i);
            ret = ERR_FILE_RESPARSE;
        }
    }

    /* check for peer */
    if(global_cfg->peer_count <= 0)
    {
        LOGE("global cfg peer count invalid[%d]", global_cfg->peer_count);
        ret = ERR_FILE_RESPARSE;
    }
    for(int i = 0; i < global_cfg->peer_count; ++i)
    {
        r_net_elem_t *elem = &global_cfg->peer_nodes[i];       

        if(!net_socket_type_valid(elem->sock_type) 
                || !util_is_ip_valid(elem->ip_str)
                || !util_is_port_valid(elem->port))
        {
            LOGE("global cfg listener[%d] invalid", i);
            ret = ERR_FILE_RESPARSE;
        }
    }

    /* check for raft */
    if(global_cfg->raft_cfg.election_timeout_min_msec >
            global_cfg->raft_cfg.election_timeout_max_msec)
    {
        LOGE("raft cfg election_timeout min[%d] over max[%d]"
                , global_cfg->raft_cfg.election_timeout_min_msec
                , global_cfg->raft_cfg.election_timeout_max_msec);
        ret = ERR_FILE_RESPARSE;
    }

    if(global_cfg->raft_cfg.loop_timeout_msec < 1)
    {  
        LOGE("raft cfg loop timeout[%d] invalid", global_cfg->raft_cfg.loop_timeout_msec);
        ret = ERR_FILE_RESPARSE;
    }

    if(global_cfg->raft_cfg.append_entry_timeout_msec <= 0)
    {
        LOGE("raft cfg append entry timeout[%d] invalid", global_cfg->raft_cfg.append_entry_timeout_msec);
        ret = ERR_FILE_RESPARSE;
    }

    return ret;
}

