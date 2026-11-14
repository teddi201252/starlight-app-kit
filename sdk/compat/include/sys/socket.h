/* libpeos-compat: BSD sockets for ports (Rift Ascend's ENet and web,
 * docs/rift-ascend-port-plan.md, phase A), over netd (peos/net.h): IPv4
 * only, UDP and outgoing TCP. A socket is a descriptor of its own; netd's
 * connection behind it is opened when the socket gets an address. No
 * listen/accept (netd has no TCP server side): EOPNOTSUPP. No AF_UNIX:
 * its absence tells ports (OpenSSL) not to look for local sockets. */
#pragma once
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/uio.h>

typedef uint32_t socklen_t;
typedef uint16_t sa_family_t;

struct sockaddr {
    sa_family_t sa_family;
    char sa_data[14];
};

struct sockaddr_storage {
    sa_family_t ss_family;
    char __ss_padding[128 - sizeof(sa_family_t) - sizeof(unsigned long)];
    unsigned long __ss_align;
};

struct msghdr {
    void *msg_name;
    socklen_t msg_namelen;
    struct iovec *msg_iov;
    size_t msg_iovlen;
    void *msg_control;
    size_t msg_controllen;
    int msg_flags;
};

struct linger {
    int l_onoff;
    int l_linger;
};

#define AF_UNSPEC 0
#define AF_INET 2
#define AF_INET6 10
#define PF_UNSPEC AF_UNSPEC
#define PF_INET AF_INET
#define PF_INET6 AF_INET6

#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOCK_RAW 3
#define SOCK_NONBLOCK 04000
#define SOCK_CLOEXEC 02000000

#define SOL_SOCKET 1
#define SO_DEBUG 1
#define SO_REUSEADDR 2
#define SO_TYPE 3
#define SO_ERROR 4
#define SO_DONTROUTE 5
#define SO_BROADCAST 6
#define SO_SNDBUF 7
#define SO_RCVBUF 8
#define SO_KEEPALIVE 9
#define SO_LINGER 13
#define SO_REUSEPORT 15
#define SO_RCVTIMEO 20
#define SO_SNDTIMEO 21

#define MSG_OOB 0x1
#define MSG_PEEK 0x2
#define MSG_TRUNC 0x20
#define MSG_DONTWAIT 0x40
#define MSG_WAITALL 0x100
#define MSG_NOSIGNAL 0x4000

#define SHUT_RD 0
#define SHUT_WR 1
#define SHUT_RDWR 2

#define SOMAXCONN 128

#ifdef __cplusplus
extern "C" {
#endif
int socket(int domain, int type, int protocol);
int bind(int fd, const struct sockaddr *addr, socklen_t len);
int connect(int fd, const struct sockaddr *addr, socklen_t len);
int listen(int fd, int backlog);
int accept(int fd, struct sockaddr *addr, socklen_t *len);
int accept4(int fd, struct sockaddr *addr, socklen_t *len, int flags);
ssize_t send(int fd, const void *buf, size_t len, int flags);
ssize_t recv(int fd, void *buf, size_t len, int flags);
ssize_t sendto(int fd, const void *buf, size_t len, int flags, const struct sockaddr *to, socklen_t tolen);
ssize_t recvfrom(int fd, void *buf, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
ssize_t sendmsg(int fd, const struct msghdr *msg, int flags);
ssize_t recvmsg(int fd, struct msghdr *msg, int flags);
int setsockopt(int fd, int level, int name, const void *val, socklen_t len);
int getsockopt(int fd, int level, int name, void *val, socklen_t *len);
int getsockname(int fd, struct sockaddr *addr, socklen_t *len);
int getpeername(int fd, struct sockaddr *addr, socklen_t *len);
int shutdown(int fd, int how);
/* No local sockets on PeOS: EOPNOTSUPP (OpenSSL's QUIC thread mode asks). */
int socketpair(int domain, int type, int protocol, int fds[2]);
#ifdef __cplusplus
}
#endif
