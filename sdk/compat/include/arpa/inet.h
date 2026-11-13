/* libpeos-compat: IPv4 addresses as text. */
#pragma once
#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif
int inet_pton(int af, const char *src, void *dst);
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
int inet_aton(const char *cp, struct in_addr *inp);
char *inet_ntoa(struct in_addr in);
in_addr_t inet_addr(const char *cp);
#ifdef __cplusplus
}
#endif
