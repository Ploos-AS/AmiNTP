#include <string.h>

#include "amintp/sntp.h"

static uint32_t read_be32(const unsigned char *p)
{
    return ((uint32_t)p[0] << 24) |
           ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) |
           (uint32_t)p[3];
}

static void write_be32(unsigned char *p, uint32_t value)
{
    p[0] = (unsigned char)(value >> 24);
    p[1] = (unsigned char)(value >> 16);
    p[2] = (unsigned char)(value >> 8);
    p[3] = (unsigned char)value;
}

static struct amintp_ntp_timestamp read_ts(const unsigned char *p)
{
    struct amintp_ntp_timestamp ts;
    ts.seconds = read_be32(p);
    ts.fraction = read_be32(p + 4);
    return ts;
}

static int timestamp_equal(const struct amintp_ntp_timestamp *a,
                           const struct amintp_ntp_timestamp *b)
{
    return a->seconds == b->seconds && a->fraction == b->fraction;
}

void amintp_sntp_build_request(unsigned char packet[AMINTP_NTP_PACKET_SIZE],
                               const struct amintp_ntp_timestamp *transmit)
{
    memset(packet, 0, AMINTP_NTP_PACKET_SIZE);
    packet[0] = (unsigned char)((AMINTP_NTP_VERSION << 3) |
                                AMINTP_NTP_MODE_CLIENT);
    if (transmit != 0) {
        write_be32(packet + 40, transmit->seconds);
        write_be32(packet + 44, transmit->fraction);
    }
}

int amintp_sntp_parse_reply(const unsigned char *packet, size_t size,
                            const struct amintp_ntp_timestamp *request_tx,
                            struct amintp_sntp_reply *reply)
{
    struct amintp_sntp_reply parsed;

    if (packet == 0 || reply == 0 || size != AMINTP_NTP_PACKET_SIZE) {
        return AMINTP_SNTP_ERR_SIZE;
    }

    parsed.leap = (packet[0] >> 6) & 0x03U;
    parsed.version = (packet[0] >> 3) & 0x07U;
    parsed.mode = packet[0] & 0x07U;
    parsed.stratum = packet[1];
    parsed.originate = read_ts(packet + 24);
    parsed.receive = read_ts(packet + 32);
    parsed.transmit = read_ts(packet + 40);

    if (parsed.mode != AMINTP_NTP_MODE_SERVER) {
        return AMINTP_SNTP_ERR_MODE;
    }
    if (parsed.version < 3U || parsed.version > AMINTP_NTP_VERSION) {
        return AMINTP_SNTP_ERR_VERSION;
    }
    if (parsed.leap == 3U) {
        return AMINTP_SNTP_ERR_LEAP;
    }
    if (parsed.stratum == 0U || parsed.stratum > 15U) {
        return AMINTP_SNTP_ERR_STRATUM;
    }
    if (request_tx != 0 && !timestamp_equal(&parsed.originate, request_tx)) {
        return AMINTP_SNTP_ERR_ORIGIN;
    }

    *reply = parsed;
    return AMINTP_SNTP_OK;
}

const char *amintp_sntp_status_string(int status)
{
    switch (status) {
    case AMINTP_SNTP_OK: return "OK";
    case AMINTP_SNTP_ERR_SIZE: return "invalid packet size";
    case AMINTP_SNTP_ERR_MODE: return "invalid server mode";
    case AMINTP_SNTP_ERR_VERSION: return "unsupported NTP version";
    case AMINTP_SNTP_ERR_LEAP: return "server clock unsynchronized";
    case AMINTP_SNTP_ERR_STRATUM: return "invalid stratum";
    case AMINTP_SNTP_ERR_ORIGIN: return "originate timestamp mismatch";
    default: return "unknown SNTP error";
    }
}
