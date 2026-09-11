#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "amintp/ipv4.h"

int amintp_parse_ipv4_literal(const char *text, struct in_addr *result)
{
    unsigned long octets[4];
    const char *p = text;
    unsigned i;
    if (text == 0 || result == 0 || *text == '\0') return 0;
    for (i = 0; i < 4; ++i) {
        unsigned long value = 0;
        unsigned digits = 0;
        if (*p < '0' || *p > '9') return 0;
        while (*p >= '0' && *p <= '9') {
            value = value * 10 + (unsigned long)(*p - '0');
            if (value > 255) return 0;
            ++digits;
            ++p;
        }
        if (digits == 0) return 0;
        octets[i] = value;
        if (i < 3) {
            if (*p != '.') return 0;
            ++p;
        }
    }
    if (*p != '\0') return 0;
    result->s_addr = htonl((uint32_t)((octets[0] << 24) |
                                      (octets[1] << 16) |
                                      (octets[2] << 8) | octets[3]));
    return 1;
}
