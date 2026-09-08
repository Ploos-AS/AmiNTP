#include <assert.h>
#include <stdio.h>

#include "amintp/cli.h"

int main(void)
{
    struct amintp_options o;
    char *query_argv[] = {"AmiNTP", "SERVER=example.org", "QUERY", "TIMEOUT=7", "RETRIES=3"};
    char *sync_argv[] = {"AmiNTP", "192.0.2.1", "SYNC", "NORTC"};
    char *bad_argv[] = {"AmiNTP", "SERVER=example.org", "QUERY", "SYNC"};
    char *bad_nortc_argv[] = {"AmiNTP", "SERVER=example.org", "QUERY", "NORTC"};
    char *bad_port_argv[] = {"AmiNTP", "SERVER=example.org", "QUERY", "PORT=65536"};
    char *bad_timeout_argv[] = {"AmiNTP", "SERVER=example.org", "QUERY", "TIMEOUT=nope"};

    assert(amintp_parse_cli(5, query_argv, &o) == 0);
    assert(o.query == 1 && o.sync == 0 && o.nortc == 0);
    assert(o.timeout_seconds == 7 && o.retries == 3);
    assert(o.timeout_set == 1 && o.retries_set == 1);
    assert(o.server_set == 1);

    assert(amintp_parse_cli(4, sync_argv, &o) == 0);
    assert(o.sync == 1 && o.query == 0 && o.nortc == 1);
    assert(o.port == 123 && o.port_set == 0);

    assert(amintp_parse_cli(4, bad_argv, &o) == 20);
    assert(amintp_parse_cli(4, bad_nortc_argv, &o) == 20);
    assert(amintp_parse_cli(4, bad_port_argv, &o) == 20);
    assert(amintp_parse_cli(4, bad_timeout_argv, &o) == 20);

    puts("M4.1 CLI tests: PASS");
    return 0;
}
