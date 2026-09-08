#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#include <exec/types.h>
#include <exec/libraries.h>
#include <proto/exec.h>
#include <proto/bsdsocket.h>
#include <libraries/bsdsocket.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include "amintp/net.h"

struct Library *SocketBase;

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

    he = gethostbyname((STRPTR)host);
    if (he == 0 || he->h_addr_list == 0 || he->h_addr_list[0] == 0) {
        CloseLibrary(SocketBase);
        SocketBase = 0;
        return 10;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    memcpy(&addr.sin_addr, he->h_addr_list[0], sizeof(addr.sin_addr));

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
            LONG got = recvfrom(fd, (char *)reply, reply_capacity, 0, 0, 0);
            if (got > 0) {
                *reply_size = (size_t)got;
                CloseSocket(fd);
                CloseLibrary(SocketBase);
                SocketBase = 0;
                return 0;
            }
        }
    }

    CloseSocket(fd);
    CloseLibrary(SocketBase);
    SocketBase = 0;
    return 10;
}
