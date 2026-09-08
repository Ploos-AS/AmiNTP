#include <stdio.h>

#include "amintp/cli.h"
#include "amintp/version.h"

int main(int argc, char **argv)
{
    struct amintp_options options;
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

    printf("AmiNTP: SERVER=%s\n", options.server);
    puts("AmiNTP: SNTP synchronization is not implemented in M0.");
    return 5;
}
