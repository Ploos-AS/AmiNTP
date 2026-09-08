#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "amintp/arexx.h"

static int query_calls;
static int sync_calls;
static int last_write_rtc;

int amintp_arexx_query_server(const char *server,
                              struct amintp_sntp_reply *reply,
                              struct amintp_amiga_time *amiga_time)
{
    ++query_calls;
    if (!strcmp(server, "bad")) return 10;
    reply->stratum = 2;
    amiga_time->seconds = 1234;
    amiga_time->micros = 500000;
    return 0;
}

int amintp_arexx_sync_server(const char *server, int write_rtc,
                             struct amintp_sntp_reply *reply,
                             struct amintp_amiga_time *amiga_time)
{
    ++sync_calls;
    last_write_rtc = write_rtc;
    if (!strcmp(server, "fail")) return 20;
    reply->stratum = 3;
    amiga_time->seconds = 5678;
    amiga_time->micros = 250000;
    return !strcmp(server, "rtcwarn") ? 5 : 0;
}

int main(void)
{
    char result[AMINTP_AREXX_RESULT_SIZE];
    amintp_arexx_reset_state();

    assert(amintp_arexx_dispatch("PING", result, sizeof(result)) == 0);
    assert(!strcmp(result, "PONG"));
    assert(amintp_arexx_dispatch("VERSION", result, sizeof(result)) == 0);
    assert(strstr(result, "0.3.2-m3.2") != 0);
    assert(amintp_arexx_dispatch("STATUS", result, sizeof(result)) == 0);

    assert(amintp_arexx_dispatch("LASTSYNC", result, sizeof(result)) == 0);
    assert(!strcmp(result, "NONE"));
    assert(amintp_arexx_dispatch("LASTERROR", result, sizeof(result)) == 0);
    assert(!strcmp(result, "NONE"));

    assert(amintp_arexx_dispatch("QUERY example.org", result, sizeof(result)) == 0);
    assert(query_calls == 1 && strstr(result, "OK QUERY SERVER=example.org") != 0);
    assert(amintp_arexx_dispatch("QUERY bad", result, sizeof(result)) == 10);
    assert(amintp_arexx_dispatch("LASTERROR", result, sizeof(result)) == 0);
    assert(strstr(result, "QUERY_FAILED") != 0);

    assert(amintp_arexx_dispatch("SYNC example.org NORTC", result, sizeof(result)) == 0);
    assert(sync_calls == 1 && last_write_rtc == 0);
    assert(strstr(result, "RTC=SKIPPED") != 0);
    assert(amintp_arexx_dispatch("LASTSYNC", result, sizeof(result)) == 0);
    assert(strstr(result, "OK SYNC SERVER=example.org") != 0);

    assert(amintp_arexx_dispatch("SYNC rtcwarn", result, sizeof(result)) == 5);
    assert(last_write_rtc == 1 && strstr(result, "RTC=FAILED") != 0);
    assert(amintp_arexx_dispatch("QUIT", result, sizeof(result)) == 0);
    assert(amintp_arexx_should_quit() == 1);

    puts("M3.2 ARexx command tests: PASS");
    return 0;
}
