#ifndef AMINTP_CLI_H
#define AMINTP_CLI_H

#define AMINTP_SERVER_MAX 127

struct amintp_options {
    const char *server;
    char server_storage[AMINTP_SERVER_MAX + 1];
    unsigned short port;
    unsigned timeout_seconds;
    unsigned retries;
    int server_set;
    int port_set;
    int timeout_set;
    int retries_set;
    int query;
    int sync;
    int nortc;
    int resident;
    int show_help;
    int show_version;
};

int amintp_parse_cli(int argc, char **argv, struct amintp_options *options);
void amintp_print_help(void);

#endif
