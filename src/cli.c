#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "amintp/cli.h"

static const char *value_after(const char *arg, const char *prefix)
{
    size_t n = strlen(prefix);
    return strncmp(arg, prefix, n) == 0 ? arg + n : 0;
}

int amintp_parse_cli(int argc, char **argv, struct amintp_options *options)
{
    int i;

    options->server = 0;
    options->port = 123;
    options->timeout_seconds = 5;
    options->retries = 2;
    options->query = 0;
    options->sync = 0;
    options->nortc = 0;
    options->show_help = 0;
    options->show_version = 0;

    for (i = 1; i < argc; ++i) {
        const char *v;

        if (!strcmp(argv[i], "?") || !strcmp(argv[i], "HELP") || !strcmp(argv[i], "--help")) {
            options->show_help = 1;
        } else if (!strcmp(argv[i], "VERSION") || !strcmp(argv[i], "--version")) {
            options->show_version = 1;
        } else if (!strcmp(argv[i], "QUERY")) {
            options->query = 1;
        } else if (!strcmp(argv[i], "SYNC")) {
            options->sync = 1;
        } else if (!strcmp(argv[i], "NORTC")) {
            options->nortc = 1;
        } else if ((v = value_after(argv[i], "SERVER=")) != 0 && *v) {
            options->server = v;
        } else if ((v = value_after(argv[i], "PORT=")) != 0 && *v) {
            options->port = (unsigned short)strtoul(v, 0, 10);
        } else if ((v = value_after(argv[i], "TIMEOUT=")) != 0 && *v) {
            options->timeout_seconds = (unsigned)strtoul(v, 0, 10);
        } else if ((v = value_after(argv[i], "RETRIES=")) != 0 && *v) {
            options->retries = (unsigned)strtoul(v, 0, 10);
        } else if (options->server == 0 && argv[i][0] != '-') {
            options->server = argv[i];
        } else {
            fprintf(stderr, "AmiNTP: unknown argument: %s\n", argv[i]);
            return 20;
        }
    }

    if (options->port == 0 || options->timeout_seconds == 0 || options->retries > 20) {
        return 20;
    }
    if (options->query && options->sync) {
        return 20;
    }
    if (options->nortc && !options->sync) {
        return 20;
    }
    return 0;
}

void amintp_print_help(void)
{
    puts("AmiNTP - minimal SNTP client for AmigaOS 2.04+");
    puts("Usage: AmiNTP SERVER=<host> QUERY [PORT=123] [TIMEOUT=5] [RETRIES=2]");
    puts("       AmiNTP SERVER=<host> SYNC  [NORTC] [PORT=123] [TIMEOUT=5] [RETRIES=2]");
    puts("       AmiNTP HELP | VERSION");
    puts("SYNC updates the system clock and, by default, battclock.resource.");
    puts("NORTC updates the system clock only.");
}
