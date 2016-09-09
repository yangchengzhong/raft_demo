#include "util_math.h"
#include <stdlib.h>
#include <time.h>

void util_srand()
{
    srand(time(NULL));
}

int util_rand(int beg, int end)
{
    if(end <= beg) return -1;

    return (rand() % (end - beg)) + beg;
}

