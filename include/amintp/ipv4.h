#ifndef AMINTP_IPV4_H
#define AMINTP_IPV4_H

#include <netinet/in.h>

int amintp_parse_ipv4_literal(const char *text, struct in_addr *result);

#endif
