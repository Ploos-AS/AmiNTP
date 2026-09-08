#include <string.h>

#include "amintp/net.h"
#include "amintp/query.h"

int amintp_query_server(const char *server, unsigned short port,
                        unsigned timeout_seconds, unsigned retries,
                        struct amintp_sntp_reply *reply)
{
    unsigned char request[AMINTP_NTP_PACKET_SIZE];
    unsigned char response[AMINTP_NTP_PACKET_SIZE];
    struct amintp_ntp_timestamp tx;
    size_t response_size = 0;
    int rc;

    memset(&tx, 0, sizeof(tx));
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
