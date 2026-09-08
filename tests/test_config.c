#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "amintp/cli.h"
#include "amintp/config.h"

static void write_text(const char *path, const char *text)
{
    FILE *fp = fopen(path, "w");
    assert(fp != 0);
    assert(fputs(text, fp) >= 0);
    assert(fclose(fp) == 0);
}

int main(void)
{
    struct amintp_options o;
    char *argv_default[] = {"AmiNTP", "QUERY"};
    char *argv_override[] = {"AmiNTP", "SERVER=cli.example", "QUERY", "PORT=8123", "TIMEOUT=9", "RETRIES=4"};
    const char *good = "build/host/m4_1_good.conf";
    const char *bad = "build/host/m4_1_bad.conf";

    write_text(good,
               "# AmiNTP M4.1 test\n"
               "SERVER = pool.example\n"
               "PORT=123\n"
               "TIMEOUT=7\n"
               "RETRIES=3\n");

    assert(amintp_parse_cli(2, argv_default, &o) == 0);
    assert(amintp_load_config(good, &o) == 0);
    assert(o.server != 0 && strcmp(o.server, "pool.example") == 0);
    assert(o.port == 123);
    assert(o.timeout_seconds == 7);
    assert(o.retries == 3);

    assert(amintp_parse_cli(6, argv_override, &o) == 0);
    assert(amintp_load_config(good, &o) == 0);
    assert(strcmp(o.server, "cli.example") == 0);
    assert(o.port == 8123);
    assert(o.timeout_seconds == 9);
    assert(o.retries == 4);

    write_text(bad, "SERVER=pool.example\nPORT=65536\n");
    assert(amintp_parse_cli(2, argv_default, &o) == 0);
    assert(amintp_load_config(bad, &o) == 20);

    assert(amintp_parse_cli(2, argv_default, &o) == 0);
    assert(amintp_load_config("build/host/does-not-exist.conf", &o) == 0);

    remove(good);
    remove(bad);
    puts("M4.1 config tests: PASS");
    return 0;
}
