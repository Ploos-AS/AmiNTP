#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "amintp/sntp.h"

static void put_be32(unsigned char *p, unsigned long v)
{
    p[0] = (unsigned char)(v >> 24);
    p[1] = (unsigned char)(v >> 16);
    p[2] = (unsigned char)(v >> 8);
    p[3] = (unsigned char)v;
}

int main(void)
{
    unsigned char request[AMINTP_NTP_PACKET_SIZE];
    unsigned char reply_packet[AMINTP_NTP_PACKET_SIZE];
    struct amintp_ntp_timestamp tx = { 0xe1234567UL, 0x89abcdefUL };
    struct amintp_sntp_reply reply;

    amintp_sntp_build_request(request, &tx);
    assert(sizeof(request) == 48U);
    assert(request[0] == 0x23U);
    assert(request[40] == 0xe1U);
    assert(request[47] == 0xefU);

    memset(reply_packet, 0, sizeof(reply_packet));
    reply_packet[0] = 0x24U; /* LI=0, VN=4, mode=server */
    reply_packet[1] = 2U;
    memcpy(reply_packet + 24, request + 40, 8);
    put_be32(reply_packet + 32, 0xe1234568UL);
    put_be32(reply_packet + 40, 0xe1234569UL);

    assert(amintp_sntp_parse_reply(reply_packet, sizeof(reply_packet), &tx,
                                   &reply) == AMINTP_SNTP_OK);
    assert(reply.version == 4U);
    assert(reply.mode == 4U);
    assert(reply.stratum == 2U);
    assert(reply.originate.seconds == tx.seconds);

    reply_packet[0] = 0xe4U;
    assert(amintp_sntp_parse_reply(reply_packet, sizeof(reply_packet), &tx,
                                   &reply) == AMINTP_SNTP_ERR_LEAP);

    reply_packet[0] = 0x24U;
    reply_packet[1] = 0U;
    assert(amintp_sntp_parse_reply(reply_packet, sizeof(reply_packet), &tx,
                                   &reply) == AMINTP_SNTP_ERR_STRATUM);

    reply_packet[1] = 2U;
    reply_packet[24] ^= 1U;
    assert(amintp_sntp_parse_reply(reply_packet, sizeof(reply_packet), &tx,
                                   &reply) == AMINTP_SNTP_ERR_ORIGIN);

    puts("M1.1 SNTP wire tests: PASS");
    return 0;
}
