#ifndef AMINTP_NET_H
#define AMINTP_NET_H

#include <stddef.h>

int amintp_udp_query(const char *host, unsigned short port,
                     const unsigned char *request, size_t request_size,
                     unsigned char *reply, size_t reply_capacity,
                     unsigned timeout_seconds, unsigned retries,
                     size_t *reply_size);

#endif
