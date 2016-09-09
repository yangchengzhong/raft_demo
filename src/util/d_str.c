#include "d_str.h"

#include <string.h>
#include <stdlib.h>

ds_t* ds_new(const uint8_t* data, uint32_t data_len)
{
    ds_t* bin = ds_new_len(data_len);
    if(NULL == bin) return bin;

    ds_append(bin, data, data_len);

    return bin;
}

ds_t* ds_new_len(const uint32_t data_len)
{
    ds_t* bin = malloc(sizeof(ds_t));
    if(NULL == bin) return NULL;

    bin->data = malloc(data_len);
    if(NULL == bin->data)
    {
        free(bin);
        return NULL;
    }

    bin->capacity = data_len;
    bin->data_len = 0;
    bin->ref_count = 1;

    return bin;
}

int ds_append(ds_t* bin, const uint8_t* data, uint32_t data_len)
{
    if(NULL == bin) return -1;
    
    if(bin->data_len + data_len > bin->capacity) return -2;

    memcpy(bin->data + bin->data_len, data, data_len);
    bin->data_len += data_len;

    return 0;
}

ds_t* ds_retain(ds_t* bin)
{
    if(NULL == bin) return NULL;

    ++bin->ref_count;

    return bin;
}

void ds_release(ds_t* bin)
{
    if(NULL == bin || bin->ref_count < 0) return ;

    if(--bin->ref_count > 0) return ;

    if(NULL != bin->data) free(bin->data);

    free(bin);

    return ;
}

