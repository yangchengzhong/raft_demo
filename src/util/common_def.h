#ifndef __COMMON_DEF_H__
#define __COMMON_DEF_H__

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "log.h"

#define UNLIKELY(expr) (!!__builtin_expect((expr), 0))
#define LIKELY(expr) (!!__builtin_expect((expr), 1))

#define assert_retval(sentence, errno)\
	do{\
		if(LIKELY((sentence)))\
			break;\
		else{\
			LOGE("assert!");\
            assert(0);\
			return (errno);\
		}\
	}while(0)

#define assert_continue(sentence)\
	do{\
		if(LIKELY((sentence)))\
			break;\
		else{\
			LOGE("assert!");\
            assert(0);\
			continue;\
		}\
	}while(0)

#define assert_noeffect(sentence)\
	do{\
		if(LIKELY((sentence)))\
			break;\
		else{\
			LOGE("assert!");\
            assert(0);\
		}\
	}while(0)

#define assert_retnone(sentence)\
	do{\
		if(LIKELY((sentence)))\
			break;\
		else{\
			LOGE("assert!");\
            assert(0);\
			return ;\
		}\
	}while(0)

#define R_MALLOC malloc
#define R_FREE free

#define OUT
#define INOUT

#define INVALID64 UINT64_MAX
#define INVALID32 UINT32_MAX

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

#endif

