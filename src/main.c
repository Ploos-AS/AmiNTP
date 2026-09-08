#include <stdio.h>

#include "amintp/cli.h"
#include "amintp/query.h"
#include "amintp/sync.h"
#include "amintp/time.h"
#include "amintp/version.h"

int main(int argc, char **argv)
{
    struct amintp_options options;
    struct amintp_sntp_reply reply;
    struct amintp_amiga_time amiga_time;
    int rc;

    rc = amintp_parse_cli(argc, argv, &options);
    if (rc != 0) return rc;
    if (options.show_version) { puts(amintp_version_string()); return 0; }
    if (options.show_help || options.server == 0) { amintp_print_help(); return 0; }

    if (!options.query && !options.sync) {
        puts("AmiNTP: specify QUERY or SYNC.");
        return 5;
    }

    rc = amintp_query_server(options.server, options.port,
                             options.timeout_seconds, options.retries,
                             &reply);
    if (rc != 0) {
        fprintf(stderr, "AmiNTP: query failed for %s\n", options.server);
        return rc;
    }

    rc = amintp_ntp_to_amiga_time(&reply.transmit, &amiga_time);
    if (rc != AMINTP_TIME_OK) {
        fprintf(stderr, "AmiNTP: invalid server time: %s\n",
                amintp_time_status_string(rc));
        return 10;
    }

    if (options.sync) {
        rc = amintp_apply_time(&amiga_time, !options.nortc);
        if (rc == 5) {
            fprintf(stderr, "WARNING SYNC SERVER=%s SYSTEM=UPDATED RTC=FAILED\n",
                    options.server);
            return 5;
        }
        if (rc != 0) {
            fprintf(stderr, "AmiNTP: failed to set system clock\n");
            return rc;
        }
        printf("OK SYNC SERVER=%s STRATUM=%u AMIGA_SECONDS=%lu AMIGA_MICROS=%lu RTC=%s\n",
               options.server, reply.stratum,
               (unsigned long)amiga_time.seconds,
               (unsigned long)amiga_time.micros,
               options.nortc ? "SKIPPED" : "UPDATED");
        return 0;
    }

    printf("OK QUERY SERVER=%s STRATUM=%u NTP_SECONDS=%lu NTP_FRACTION=%lu "
           "AMIGA_SECONDS=%lu AMIGA_MICROS=%lu\n",
           options.server, reply.stratum,
           (unsigned long)reply.transmit.seconds,
           (unsigned long)reply.transmit.fraction,
           (unsigned long)amiga_time.seconds,
           (unsigned long)amiga_time.micros);
    return 0;
}
