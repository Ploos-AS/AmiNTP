#include "amintp/arexx.h"
#include "amintp/query.h"
#include "amintp/sync.h"
#include "amintp/time.h"

int amintp_arexx_query_server(const char *server,
                              struct amintp_sntp_reply *reply,
                              struct amintp_amiga_time *amiga_time)
{
    int rc = amintp_query_server(server, 123, 5, 2, reply);
    if (rc != 0) return rc;
    rc = amintp_ntp_to_amiga_time(&reply->transmit, amiga_time);
    return rc == AMINTP_TIME_OK ? 0 : 10;
}

int amintp_arexx_sync_server(const char *server, int write_rtc,
                             struct amintp_sntp_reply *reply,
                             struct amintp_amiga_time *amiga_time)
{
    int rc = amintp_arexx_query_server(server, reply, amiga_time);
    if (rc != 0) return rc;
    return amintp_apply_time(amiga_time, write_rtc);
}
