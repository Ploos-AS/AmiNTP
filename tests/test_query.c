#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "amintp/net.h"
#include "amintp/query.h"
#include "amintp/sntp.h"

static int mock_mode;

int amintp_udp_query(const char *host, unsigned short port,
                     const unsigned char *request, size_t request_size,
                     unsigned char *reply, size_t reply_capacity,
                     unsigned timeout_seconds, unsigned retries,
                     size_t *reply_size)
{
    (void)host;
    (void)port;
    (void)timeout_seconds;
    (void)retries;

    assert(request_size == AMINTP_NTP_PACKET_SIZE);
    assert(reply_capacity >= AMINTP_NTP_PACKET_SIZE);

    if (mock_mode == 1) {
        return 10;
    }

    memset(reply, 0, AMINTP_NTP_PACKET_SIZE);

    if (mock_mode == 2) {
        reply[0] = (unsigned char)((AMINTP_NTP_VERSION << 3) | 3); /* client mode: invalid reply */
        reply[1] = 2;
        *reply_size = AMINTP_NTP_PACKET_SIZE;
        return 0;
    }

    reply[0] = (unsigned char)((AMINTP_NTP_VERSION << 3) | AMINTP_NTP_MODE_SERVER);
    reply[1] = 2;
    memcpy(reply + 24, request + 40, 8); /* originate echoes request transmit */
    reply[40] = 0xe9;
    reply[41] = 0x50;
    reply[42] = 0x00;
    reply[43] = 0x01;
    *reply_size = AMINTP_NTP_PACKET_SIZE;
    return 0;
}

int main(void)
{
    struct amintp_sntp_reply reply;

    mock_mode = 0;
    assert(amintp_query_server("example.invalid", 123, 1, 0, &reply) == 0);
    assert(reply.stratum == 2);

    mock_mode = 1;
    assert(amintp_query_server("example.invalid", 123, 1, 0, &reply) == 10);

    mock_mode = 2;
    assert(amintp_query_server("example.invalid", 123, 1, 0, &reply) == 10);

    puts("M1.3 query tests: PASS");
    return 0;
}
