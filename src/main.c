#include <stdio.h>

#include "amintp/cli.h"
#include "amintp/query.h"
#include "amintp/version.h"

int main(int argc, char **argv)
{
    struct amintp_options options;
    struct amintp_sntp_reply reply;
    int rc;

    rc = amintp_parse_cli(argc, argv, &options);
    if (rc != 0) {
        return rc;
    }

    if (options.show_version) {
        puts(amintp_version_string());
        return 0;
    }

    if (options.show_help || options.server == 0) {
        amintp_print_help();
        return 0;
    }

    if (!options.query) {
        puts("AmiNTP: M1.2 only supports QUERY; clock setting arrives in M2.");
        return 5;
    }

    rc = amintp_query_server(options.server, options.port,
                             options.timeout_seconds, options.retries,
                             &reply);
    if (rc != 0) {
        fprintf(stderr, "AmiNTP: query failed for %s\n", options.server);
        return rc;
    }

    printf("OK SERVER=%s STRATUM=%u NTP_SECONDS=%lu NTP_FRACTION=%lu\n",
           options.server, reply.stratum,
           (unsigned long)reply.transmit.seconds,
           (unsigned long)reply.transmit.fraction);
    return 0;
}
