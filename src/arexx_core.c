#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "amintp/arexx.h"
#include "amintp/platform.h"
#include "amintp/version.h"

static int quit_requested;
static char last_sync[AMINTP_AREXX_RESULT_SIZE] = "NONE";
static char last_error[AMINTP_AREXX_RESULT_SIZE] = "NONE";

static const char *skip_space(const char *s)
{
    while (s != 0 && *s != '\0' && isspace((unsigned char)*s)) ++s;
    return s;
}

static void read_token(const char **cursor, char *dst, size_t dst_size, int upper)
{
    const char *s = skip_space(*cursor);
    size_t n = 0;
    if (dst_size == 0) return;
    while (s != 0 && *s != '\0' && !isspace((unsigned char)*s) && n + 1 < dst_size) {
        dst[n++] = upper ? (char)toupper((unsigned char)*s) : *s;
        ++s;
    }
    dst[n] = '\0';
    *cursor = s;
}

static int fail(char *result, size_t result_size, int rc, const char *code)
{
    snprintf(result, result_size, "ERROR CODE=%s", code);
    snprintf(last_error, sizeof(last_error), "%s", result);
    return rc;
}

void amintp_arexx_reset_state(void)
{
    quit_requested = 0;
    snprintf(last_sync, sizeof(last_sync), "NONE");
    snprintf(last_error, sizeof(last_error), "NONE");
}

int amintp_arexx_should_quit(void)
{
    return quit_requested;
}

int amintp_arexx_dispatch(const char *command, char *result, size_t result_size)
{
    const char *cursor = command;
    char verb[32];
    char server[128];
    char option[32];
    struct amintp_sntp_reply reply;
    struct amintp_amiga_time amiga_time;
    int rc;

    if (result == 0 || result_size == 0) return 20;
    result[0] = '\0';
    read_token(&cursor, verb, sizeof(verb), 1);

    if (!strcmp(verb, "PING")) { snprintf(result, result_size, "PONG"); return 0; }
    if (!strcmp(verb, "VERSION")) { snprintf(result, result_size, "%s", amintp_version_string()); return 0; }
    if (!strcmp(verb, "STATUS")) {
        snprintf(result, result_size, "OK MODE=RESIDENT PORT=%s", AMINTP_AREXX_PORT);
        return 0;
    }
    if (!strcmp(verb, "LASTSYNC")) { snprintf(result, result_size, "%s", last_sync); return 0; }
    if (!strcmp(verb, "LASTERROR")) { snprintf(result, result_size, "%s", last_error); return 0; }
    if (!strcmp(verb, "QUIT")) {
        quit_requested = 1;
        snprintf(result, result_size, "OK QUIT");
        return 0;
    }
    if (!strcmp(verb, "QUERY") || !strcmp(verb, "SYNC")) {
        int write_rtc = 1;
        read_token(&cursor, server, sizeof(server), 0);
        if (server[0] == '\0') return fail(result, result_size, 20, "SERVER_REQUIRED");
        if (!strncmp(server, "SERVER=", 7) || !strncmp(server, "server=", 7)) {
            memmove(server, server + 7, strlen(server + 7) + 1);
        }
        if (server[0] == '\0') return fail(result, result_size, 20, "SERVER_REQUIRED");
        read_token(&cursor, option, sizeof(option), 1);
        if (!strcmp(verb, "QUERY")) {
            if (option[0] != '\0') return fail(result, result_size, 20, "BAD_ARGUMENT");
            rc = amintp_arexx_query_server(server, &reply, &amiga_time);
            if (rc != 0) return fail(result, result_size, rc, "QUERY_FAILED");
            snprintf(result, result_size,
                     "OK QUERY SERVER=%s STRATUM=%u AMIGA_SECONDS=%lu AMIGA_MICROS=%lu",
                     server, reply.stratum, (unsigned long)amiga_time.seconds,
                     (unsigned long)amiga_time.micros);
            return 0;
        }
        if (option[0] != '\0') {
            if (strcmp(option, "NORTC")) return fail(result, result_size, 20, "BAD_ARGUMENT");
            write_rtc = 0;
        }
        rc = amintp_arexx_sync_server(server, write_rtc, &reply, &amiga_time);
        if (rc != 0 && rc != 5) return fail(result, result_size, rc, "SYNC_FAILED");
        snprintf(result, result_size,
                 "%s SYNC SERVER=%s STRATUM=%u AMIGA_SECONDS=%lu AMIGA_MICROS=%lu RTC=%s",
                 rc == 5 ? "WARNING" : "OK", server, reply.stratum,
                 (unsigned long)amiga_time.seconds, (unsigned long)amiga_time.micros,
                 write_rtc ? (rc == 5 ? "FAILED" : "UPDATED") : "SKIPPED");
        snprintf(last_sync, sizeof(last_sync), "%s", result);
        if (rc == 5) snprintf(last_error, sizeof(last_error), "ERROR CODE=RTC_FAILED");
        return rc;
    }
    if (verb[0] == '\0') return fail(result, result_size, 20, "EMPTY_COMMAND");

    snprintf(result, result_size, "ERROR CODE=UNKNOWN_COMMAND COMMAND=%s", verb);
    snprintf(last_error, sizeof(last_error), "%s", result);
    return 10;
}
