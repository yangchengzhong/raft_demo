#include "res_mng.h"

static RES_MNG_UNIT_DEF *gs_def_container = NULL;
static int gs_def_count = 0; 

static int _res_init();
static int _res_load();
static int _res_parse();
static int _res_check();
static int _res_assemble();

static int _res_def_reg(RES_MNG_UNIT_DEF def_arr[], const int def_count);
static int _res_mng_init();

int res_mng_init(RES_MNG_UNIT_DEF def_arr[]
        , const int def_count)
{
    /* 1. reg */
    int ret = _res_def_reg(def_arr, def_count);
    if(0 != ret) 
    {
        LOGE("fail to reg, ret = %d", ret);
        return ret;
    }

    /* 2. do init */
    ret = _res_mng_init();
    if(0 != ret) 
    {
        LOGE("fail to init, ret = %d", ret);
        return ret;
    }

    return 0;
}
/*********************** static functions ************************/
static int _res_load_read_file(config_t *config
        , const char* file_name)
{
    assert_retval(NULL != config && NULL != file_name, ERR_INVALID_ARG);

    FILE *fp = fopen(file_name,"r");
    if(NULL == fp) 
    {
        LOGE("can't open file '%s'.", file_name);
        return ERR_FILE_NOTFOUND;
    }

    fclose(fp);
    
    int ret = config_read_file(config, file_name);
    if(CONFIG_FALSE == ret)
    {
        LOGE("%s:%d - %s, ret = %d"
                , config_error_file(config)
                , config_error_line(config)
                , config_error_text(config)
                , ret);
        return ERR_FILE_RESLOAD;
    }

    return 0;
}

static int _res_init()
{
    for(int i = 0; i < gs_def_count; ++i)
    {
        config_init(&gs_def_container[i].conf);
    }

    return 0;
}

static int _res_load()
{
    int ret = -1; 

    for(int i = 0; i < gs_def_count; ++i)
    {
        if(NULL == gs_def_container[i].cfg_getter()) continue;

        ret = _res_load_read_file(&gs_def_container[i].conf
                , gs_def_container[i].cfg_getter());
        if(0 != ret)
        {
            LOGE("fail to load res");
            return ret;
        }
    }

    return 0;
}

static int _res_parse()
{
    int ret = -1; 

    for(int i = 0; i < gs_def_count; ++i)
    {
        if(NULL == gs_def_container[i].cfg_getter()) continue;

        RES_MNG_UNIT_DEF *unit_def = &gs_def_container[i];
        assert_retval(NULL != unit_def 
                && NULL != unit_def->parse_pfn
                , ERR_INVALID_ARG);
        ret = unit_def->parse_pfn(&unit_def->conf);
        if(0 != ret)
        {
            LOGE("fail to pase res[%d]", i);
            return ret;
        }
    }

    return 0;
}

static int _res_check()
{
    int ret = -1; 

    for(int i = 0; i < gs_def_count; ++i)
    {
        if(NULL == gs_def_container[i].cfg_getter()) continue;

        RES_MNG_UNIT_DEF *unit_def = &gs_def_container[i];
        assert_retval(NULL != unit_def 
                && NULL != unit_def->check_pfn
                , ERR_INVALID_ARG);
        ret = unit_def->check_pfn();
        if(0 != ret)
        {
            LOGE("fail to check res[%zu]", i);
            return ret;
        }
    }

    return 0;
}

static int _res_assemble()
{
    int ret = -1; 

    for(int i = 0; i < gs_def_count; ++i)
    {
        if(NULL == gs_def_container[i].cfg_getter()) continue;

        RES_MNG_UNIT_DEF *unit_def = &gs_def_container[i];
        assert_retval(NULL != unit_def, ERR_INVALID_ARG); 

        if(NULL == unit_def->assemble_pfn) continue;
        ret = unit_def->assemble_pfn();
        if(0 != ret)
        {
            LOGE("fail to assemble res[%zu]", i);
            return ret;
        }
    }

    return 0;
}

int _res_def_reg(RES_MNG_UNIT_DEF def_arr[], const int def_count)
{
    assert_retval(NULL != def_arr && def_count >= 0, ERR_INVALID_ARG);

    gs_def_container = def_arr;
    gs_def_count = def_count;

    return 0;
}

int _res_mng_init()
{
    /* 1. res init */
    int ret = _res_init();
    if(0 != ret)
    {
        LOGE("res init fail");
        return ret;
    }

    /* 2. res load */
    ret = _res_load();
    if(0 != ret)
    {
        LOGE("res load fail");
        return ret;
    }

    /* 3. res parser */
    ret = _res_parse();
    if(0 != ret)
    {
        LOGE("res parse fail");
        return ret;
    }

    /* 4. res check */
    ret = _res_check();
    if(0 != ret)
    {
        LOGE("res check fail");    
        return ret;
    }

    /* 5. res assemble */
    ret = _res_assemble();
    if(0 != ret)
    {
        LOGE("res assemble fail");    
        return ret;
    }

    return 0;
}

//static void _res_clear_temp()
//{
//    for(int i = 0; i < gs_def_count; ++i)
//    {
//        config_destroy(&gs_def_container[i].conf);
//    }
//}
