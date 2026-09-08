#ifndef AMINTP_CLI_H
#define AMINTP_CLI_H

struct amintp_options {
    const char *server;
    unsigned short port;
    unsigned timeout_seconds;
    unsigned retries;
    int query;
    int sync;
    int nortc;
    int show_help;
    int show_version;
};

int amintp_parse_cli(int argc, char **argv, struct amintp_options *options);
void amintp_print_help(void);

#endif
