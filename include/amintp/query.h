#ifndef AMINTP_QUERY_H
#define AMINTP_QUERY_H

#include "amintp/sntp.h"

int amintp_query_server(const char *server, unsigned short port,
                        unsigned timeout_seconds, unsigned retries,
                        struct amintp_sntp_reply *reply);

#endif
