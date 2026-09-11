#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#include <exec/types.h>
#include <exec/libraries.h>
#include <proto/exec.h>
#define SocketBase AmiNTPSocketBase
#include <proto/bsdsocket.h>
#include <libraries/bsdsocket.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include "amintp/net.h"
#include "amintp/ipv4.h"

struct Library *AmiNTPSocketBase;

int amintp_udp_query(const char *host, unsigned short port,
                     const unsigned char *request, size_t request_size,
                     unsigned char *reply, size_t reply_capacity,
                     unsigned timeout_seconds, unsigned retries,
                     size_t *reply_size)
{
    struct hostent *he;
    struct sockaddr_in addr;
    unsigned attempt;
    LONG fd;

    SocketBase = OpenLibrary((CONST_STRPTR)"bsdsocket.library", 4);
    if (SocketBase == 0) {
        return 10;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (!amintp_parse_ipv4_literal(host, &addr.sin_addr)) {
        he = gethostbyname((STRPTR)host);
        if (he == 0 || he->h_addr_list == 0 || he->h_addr_list[0] == 0 ||
            he->h_addrtype != AF_INET || he->h_length != sizeof(addr.sin_addr)) {
            CloseLibrary(SocketBase); SocketBase = 0; return 10;
        }
        memcpy(&addr.sin_addr, he->h_addr_list[0], sizeof(addr.sin_addr));
    }
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        CloseLibrary(SocketBase);
        SocketBase = 0;
        return 10;
    }

    for (attempt = 0; attempt <= retries; ++attempt) {
        fd_set readfds;
        struct timeval tv;
        LONG ready;

        if (sendto(fd, (char *)request, request_size, 0,
                   (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            continue;
        }

        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        tv.tv_sec = timeout_seconds;
        tv.tv_usec = 0;
        ready = WaitSelect(fd + 1, &readfds, 0, 0, &tv, 0);
        if (ready > 0 && FD_ISSET(fd, &readfds)) {
            struct sockaddr_in sender;
            socklen_t sender_size = sizeof(sender);
            LONG got;
            memset(&sender, 0, sizeof(sender));
            got = recvfrom(fd, (char *)reply, reply_capacity, 0,
                           (struct sockaddr *)&sender, &sender_size);
            if (got > 0) {
                *reply_size = (size_t)got;
                CloseSocket(fd);
                CloseLibrary(SocketBase);
                SocketBase = 0;
                /* Fail closed on an unrelated peer, including a wrong UDP port. */
                return sender_size == sizeof(sender) &&
                       sender.sin_family == AF_INET &&
                       sender.sin_addr.s_addr == addr.sin_addr.s_addr &&
                       sender.sin_port == addr.sin_port ? 0 : 10;
            }
        }
    }

    CloseSocket(fd);
    CloseLibrary(SocketBase);
    SocketBase = 0;
    return 10;
}
