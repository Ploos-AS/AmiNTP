#ifndef AMINTP_SNTP_H
#define AMINTP_SNTP_H

#include <stddef.h>
#include <stdint.h>

#define AMINTP_NTP_PACKET_SIZE 48
#define AMINTP_NTP_VERSION 4
#define AMINTP_NTP_MODE_CLIENT 3
#define AMINTP_NTP_MODE_SERVER 4

enum amintp_sntp_status {
    AMINTP_SNTP_OK = 0,
    AMINTP_SNTP_ERR_SIZE = -1,
    AMINTP_SNTP_ERR_MODE = -2,
    AMINTP_SNTP_ERR_VERSION = -3,
    AMINTP_SNTP_ERR_LEAP = -4,
    AMINTP_SNTP_ERR_STRATUM = -5,
    AMINTP_SNTP_ERR_ORIGIN = -6
};

struct amintp_ntp_timestamp {
    uint32_t seconds;
    uint32_t fraction;
};

struct amintp_sntp_reply {
    unsigned leap;
    unsigned version;
    unsigned mode;
    unsigned stratum;
    struct amintp_ntp_timestamp originate;
    struct amintp_ntp_timestamp receive;
    struct amintp_ntp_timestamp transmit;
};

void amintp_sntp_build_request(unsigned char packet[AMINTP_NTP_PACKET_SIZE],
                               const struct amintp_ntp_timestamp *transmit);
int amintp_sntp_parse_reply(const unsigned char *packet, size_t size,
                            const struct amintp_ntp_timestamp *request_tx,
                            struct amintp_sntp_reply *reply);
const char *amintp_sntp_status_string(int status);

#endif
