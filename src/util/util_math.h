#ifndef __UTIL_MATH_H__
#define __UTIL_MATH_H__

#define MAX(a,b) ((a)>(b) ? a : b)
#define MIN(a,b) ((a)>(b) ? b : a)

void util_srand();

int util_rand(int beg, int end);

#endif


