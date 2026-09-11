#include <assert.h>
#include <arpa/inet.h>
#include <string.h>
#include "amintp/ipv4.h"
static void valid(const char *s, unsigned long v){ struct in_addr a; assert(amintp_parse_ipv4_literal(s,&a)); assert(ntohl(a.s_addr)==v); }
static void invalid(const char *s){ struct in_addr a; assert(!amintp_parse_ipv4_literal(s,&a)); }
int main(void){ valid("0.0.0.0",0); valid("1.2.3.4",0x01020304UL); valid("127.0.0.1",0x7f000001UL); valid("192.168.1.1",0xc0a80101UL); valid("255.255.255.255",0xffffffffUL); invalid("256.1.1.1"); invalid("1.256.1.1"); invalid("1.2.3"); invalid("1.2.3.4.5"); invalid("1..3.4"); invalid("-1.2.3.4"); invalid("1.2.3.x"); invalid("1.2.3.4x"); invalid(""); invalid("pool.ntp.org"); invalid("ntp.example.org"); return 0; }
