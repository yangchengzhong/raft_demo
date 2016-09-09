#ifndef __RES_MNG_H__
#define __RES_MNG_H__

#include "common_inc.h"
#include "libconfig.h"

typedef int (*pfn_res_parser)(config_t *);
typedef int (*pfn_res_checker)();
typedef int (*pfn_res_assembler)();
typedef const char* (*pfn_res_cfg_getter)();

typedef struct 
{
    int                  res_id;
    config_t             conf;
    pfn_res_cfg_getter   cfg_getter;
    pfn_res_parser       parse_pfn;
    pfn_res_checker      check_pfn;
    pfn_res_assembler    assemble_pfn;
}RES_MNG_UNIT_DEF;

int res_mng_init(RES_MNG_UNIT_DEF def_arr[], const int def_count);

#endif

