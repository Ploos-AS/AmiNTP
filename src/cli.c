#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "amintp/cli.h"

static const char *value_after(const char *arg, const char *prefix)
{
    size_t n = strlen(prefix);
    return strncmp(arg, prefix, n) == 0 ? arg + n : 0;
}

static int parse_unsigned(const char *text, unsigned long max_value, unsigned long *out)
{
    char *end = 0;
    unsigned long value;

    if (text == 0 || *text == '\0') return 20;
    value = strtoul(text, &end, 10);
    if (end == text || *end != '\0' || value > max_value) return 20;
    *out = value;
    return 0;
}

static int set_server(struct amintp_options *options, const char *server)
{
    size_t n;
    if (server == 0 || *server == '\0') return 20;
    n = strlen(server);
    if (n > AMINTP_SERVER_MAX) return 20;
    memcpy(options->server_storage, server, n + 1);
    options->server = options->server_storage;
    options->server_set = 1;
    return 0;
}

int amintp_parse_cli(int argc, char **argv, struct amintp_options *options)
{
    int i;

    options->server = 0;
    options->server_storage[0] = '\0';
    options->port = 123;
    options->timeout_seconds = 5;
    options->retries = 2;
    options->server_set = 0;
    options->port_set = 0;
    options->timeout_set = 0;
    options->retries_set = 0;
    options->query = 0;
    options->sync = 0;
    options->nortc = 0;
    options->resident = 0;
    options->show_help = 0;
    options->show_version = 0;

    for (i = 1; i < argc; ++i) {
        const char *v;
        unsigned long value;

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
        } else if (!strcmp(argv[i], "RESIDENT")) {
            options->resident = 1;
        } else if ((v = value_after(argv[i], "SERVER=")) != 0 && *v) {
            if (set_server(options, v) != 0) return 20;
        } else if ((v = value_after(argv[i], "PORT=")) != 0 && *v) {
            if (parse_unsigned(v, 65535UL, &value) != 0 || value == 0) return 20;
            options->port = (unsigned short)value;
            options->port_set = 1;
        } else if ((v = value_after(argv[i], "TIMEOUT=")) != 0 && *v) {
            if (parse_unsigned(v, 3600UL, &value) != 0 || value == 0) return 20;
            options->timeout_seconds = (unsigned)value;
            options->timeout_set = 1;
        } else if ((v = value_after(argv[i], "RETRIES=")) != 0 && *v) {
            if (parse_unsigned(v, 20UL, &value) != 0) return 20;
            options->retries = (unsigned)value;
            options->retries_set = 1;
        } else if (options->server == 0 && argv[i][0] != '-') {
            if (set_server(options, argv[i]) != 0) return 20;
        } else {
            fprintf(stderr, "AmiNTP: unknown argument: %s\n", argv[i]);
            return 20;
        }
    }

    if (options->query && options->sync) return 20;
    if (options->nortc && !options->sync) return 20;
    if (options->resident && (options->query || options->sync || options->nortc)) return 20;
    return 0;
}

void amintp_print_help(void)
{
    puts("AmiNTP - minimal SNTP client for AmigaOS 2.04+");
    puts("Usage: AmiNTP [SERVER=<host>] QUERY [PORT=123] [TIMEOUT=5] [RETRIES=2]");
    puts("       AmiNTP [SERVER=<host>] SYNC  [NORTC] [PORT=123] [TIMEOUT=5] [RETRIES=2]");
    puts("       AmiNTP RESIDENT");
    puts("       AmiNTP HELP | VERSION");
    puts("Defaults may be read from ENVARC:AmiNTP/AmiNTP.conf; CLI values override config.");
    puts("RESIDENT creates the public ARexx port AMINTP.");
}
