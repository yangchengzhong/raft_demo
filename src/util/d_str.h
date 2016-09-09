#ifndef __D_STR_H__
#define __D_STR_H__

#include <stdint.h>

typedef struct
{
    uint8_t*    data;
    uint32_t    data_len;
    uint32_t    capacity;
    uint16_t    ref_count;
}ds_t;

ds_t* ds_new(const uint8_t* data, uint32_t data_len);
ds_t* ds_new_len(const uint32_t data_len);

int ds_append(ds_t* bin, const uint8_t* data, uint32_t data_len);

ds_t* ds_retain(ds_t* bin);

void ds_release(ds_t* bin);

#endif

