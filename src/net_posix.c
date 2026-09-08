#ifndef __amigaos__

#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include "amintp/net.h"

int amintp_udp_query(const char *host, unsigned short port,
                     const unsigned char *request, size_t request_size,
                     unsigned char *reply, size_t reply_capacity,
                     unsigned timeout_seconds, unsigned retries,
                     size_t *reply_size)
{
    struct hostent *he;
    struct sockaddr_in addr;
    unsigned attempt;
    int fd;

    he = gethostbyname(host);
    if (he == 0 || he->h_addr_list == 0 || he->h_addr_list[0] == 0 ||
        he->h_addrtype != AF_INET || he->h_length != sizeof(addr.sin_addr)) {
        return 10;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    memcpy(&addr.sin_addr, he->h_addr_list[0], sizeof(addr.sin_addr));

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        return 10;
    }

    for (attempt = 0; attempt <= retries; ++attempt) {
        fd_set readfds;
        struct timeval tv;
        int ready;

        if (sendto(fd, request, request_size, 0,
                   (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            continue;
        }

        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        tv.tv_sec = (long)timeout_seconds;
        tv.tv_usec = 0;
        ready = select(fd + 1, &readfds, 0, 0, &tv);
        if (ready > 0 && FD_ISSET(fd, &readfds)) {
            struct sockaddr_in sender;
            socklen_t sender_size = sizeof(sender);
            ssize_t got;
            memset(&sender, 0, sizeof(sender));
            got = recvfrom(fd, reply, reply_capacity, 0,
                           (struct sockaddr *)&sender, &sender_size);
            if (got > 0) {
                *reply_size = (size_t)got;
                close(fd);
                /* Fail closed on an unrelated peer, including a wrong UDP port. */
                return sender_size == sizeof(sender) &&
                       sender.sin_family == AF_INET &&
                       sender.sin_addr.s_addr == addr.sin_addr.s_addr &&
                       sender.sin_port == addr.sin_port ? 0 : 10;
            }
        }
    }

    close(fd);
    return 10;
}

#endif
