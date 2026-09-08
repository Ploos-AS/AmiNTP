#include <stdio.h>
#include <string.h>

#include "amintp/cli.h"

static const char *server_value(const char *arg)
{
    static const char prefix[] = "SERVER=";

    if (strncmp(arg, prefix, sizeof(prefix) - 1) == 0) {
        return arg + sizeof(prefix) - 1;
    }

    return 0;
}

int amintp_parse_cli(int argc, char **argv, struct amintp_options *options)
{
    int i;

    options->server = 0;
    options->show_help = 0;
    options->show_version = 0;

    for (i = 1; i < argc; ++i) {
        const char *value;

        if (strcmp(argv[i], "?") == 0 || strcmp(argv[i], "HELP") == 0 ||
            strcmp(argv[i], "--help") == 0) {
            options->show_help = 1;
            continue;
        }

        if (strcmp(argv[i], "VERSION") == 0 ||
            strcmp(argv[i], "--version") == 0) {
            options->show_version = 1;
            continue;
        }

        value = server_value(argv[i]);
        if (value != 0 && value[0] != '\0') {
            options->server = value;
            continue;
        }

        if (options->server == 0 && argv[i][0] != '-') {
            options->server = argv[i];
            continue;
        }

        fprintf(stderr, "AmiNTP: unknown argument: %s\n", argv[i]);
        return 20;
    }

    return 0;
}

void amintp_print_help(void)
{
    puts("AmiNTP - minimal SNTP client for AmigaOS 2.04+");
    puts("");
    puts("Usage:");
    puts("  AmiNTP SERVER");
    puts("  AmiNTP SERVER=<host>");
    puts("  AmiNTP HELP");
    puts("  AmiNTP VERSION");
    puts("");
    puts("M0 note: SNTP network operations are not implemented yet.");
}
