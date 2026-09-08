#include <string.h>
#include <sys/time.h>

#include "amintp/net.h"
#include "amintp/query.h"

int amintp_query_server(const char *server, unsigned short port,
                        unsigned timeout_seconds, unsigned retries,
                        struct amintp_sntp_reply *reply)
{
    unsigned char request[AMINTP_NTP_PACKET_SIZE];
    /* One extra byte preserves evidence of oversized UDP datagrams. */
    unsigned char response[AMINTP_NTP_PACKET_SIZE + 1];
    struct amintp_ntp_timestamp tx;
    size_t response_size = 0;
    struct timeval now;
    static struct amintp_ntp_timestamp previous;
    int rc;

    if (gettimeofday(&now, 0) != 0) return 10;
    /* Correlation, not authentication: the server must echo this exact value.
     * Keep consecutive queries distinct even within one clock tick or after
     * a backwards system-clock adjustment. */
    tx.seconds = (uint32_t)now.tv_sec + 2208988800UL;
    tx.fraction = (uint32_t)(((uint64_t)now.tv_usec << 32) / 1000000UL);
    if (tx.seconds < previous.seconds ||
        (tx.seconds == previous.seconds && tx.fraction <= previous.fraction)) {
        tx = previous;
        if (++tx.fraction == 0) ++tx.seconds;
    }
    if (tx.seconds == 0 && tx.fraction == 0) tx.fraction = 1;
    previous = tx;
    amintp_sntp_build_request(request, &tx);

    rc = amintp_udp_query(server, port,
                          request, sizeof(request),
                          response, sizeof(response),
                          timeout_seconds, retries,
                          &response_size);
    if (rc != 0) {
        return rc;
    }

    rc = amintp_sntp_parse_reply(response, response_size, &tx, reply);
    if (rc != AMINTP_SNTP_OK) {
        return 10;
    }

    return 0;
}
