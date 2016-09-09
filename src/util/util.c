#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

uint64_t util_sockaddr_2_u64(const struct sockaddr* addr)
{
    if(NULL == addr) return UINT64_MAX;

    struct sockaddr_in* sa = (struct sockaddr_in*)addr;

    return (sa->sin_addr.s_addr << 4) + (ntohs(sa->sin_port) << 2);
}

uint64_t util_sockipport_2_u64(const char* ip, uint16_t port)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    return util_sockaddr_2_u64((const struct sockaddr*)&addr);
}

bool util_is_ip_valid(const char* ip)
{
    if(NULL == ip) return false;

    return INADDR_NONE == inet_addr(ip)
        ? false
        : true;
}

bool util_is_port_valid(uint16_t port)
{
    return port > 0 && port <= 65535
        ? true
        : false;
}

const char* util_ip_int2str(const uint32_t ip_int)
{
    struct sockaddr_in sa;
    sa.sin_addr.s_addr = ip_int;

    return inet_ntoa(sa.sin_addr);
}

uint64_t util_now()
{
	struct timeval cur_time;
	gettimeofday(&cur_time, NULL);

	return cur_time.tv_sec*1000 + cur_time.tv_usec/1000;
}

