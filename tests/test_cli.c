#include <assert.h>
#include <stdio.h>

#include "amintp/cli.h"

int main(void)
{
    struct amintp_options o;
    char *query_argv[] = {"AmiNTP", "SERVER=example.org", "QUERY", "TIMEOUT=7", "RETRIES=3"};
    char *sync_argv[] = {"AmiNTP", "192.0.2.1", "SYNC"};
    char *bad_argv[] = {"AmiNTP", "SERVER=example.org", "QUERY", "SYNC"};

    assert(amintp_parse_cli(5, query_argv, &o) == 0);
    assert(o.query == 1 && o.sync == 0);
    assert(o.timeout_seconds == 7 && o.retries == 3);

    assert(amintp_parse_cli(3, sync_argv, &o) == 0);
    assert(o.sync == 1 && o.query == 0);
    assert(o.port == 123);

    assert(amintp_parse_cli(4, bad_argv, &o) == 20);

    puts("M2.2 CLI tests: PASS");
    return 0;
}
