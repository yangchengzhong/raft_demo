#ifndef __R_RES_MNG_H__
#define __R_RES_MNG_H__

typedef enum 
{
    R_RES_ID_NONE = -1,
    R_RES_ID_GLOBAL_CFG,
    R_RES_ID_MAX,
}R_RES_ID;

int r_res_mng_init();

#endif

