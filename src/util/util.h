#ifndef __UTIL_H__
#define __UTIL_H__

#include <netinet/in.h>
#include <stdbool.h>

#define container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - offsetof(type, member)))

uint64_t util_sockaddr_2_u64(const struct sockaddr* addr);

uint64_t util_sockipport_2_u64(const char* ip, uint16_t port);

bool util_is_ip_valid(const char* ip);

bool util_is_port_valid(uint16_t port);

const char* util_ip_int2str(const uint32_t ip_int);

uint64_t util_now();

#endif

