#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "amintp/arexx.h"

int main(void)
{
    char result[AMINTP_AREXX_RESULT_SIZE];

    assert(amintp_arexx_dispatch("PING", result, sizeof(result)) == 0);
    assert(!strcmp(result, "PONG"));

    assert(amintp_arexx_dispatch(" version ", result, sizeof(result)) == 0);
    assert(strstr(result, "AmiNTP 0.3.1-m3.1") != 0);

    assert(amintp_arexx_dispatch("STATUS", result, sizeof(result)) == 0);
    assert(!strcmp(result, "OK MODE=RESIDENT PORT=AMINTP"));

    assert(amintp_arexx_dispatch("NOPE", result, sizeof(result)) == 10);
    assert(strstr(result, "UNKNOWN_COMMAND") != 0);

    assert(amintp_arexx_dispatch("", result, sizeof(result)) == 20);
    assert(strstr(result, "EMPTY_COMMAND") != 0);

    puts("M3.1 ARexx dispatcher tests: PASS");
    return 0;
}
