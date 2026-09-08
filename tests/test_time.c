#include <assert.h>
#include <stdio.h>

#include "amintp/time.h"

int main(void)
{
    struct amintp_ntp_timestamp ntp;
    struct amintp_amiga_time out;

    ntp.seconds = AMINTP_NTP_TO_AMIGA_EPOCH;
    ntp.fraction = 0;
    assert(amintp_ntp_to_amiga_time(&ntp, &out) == AMINTP_TIME_OK);
    assert(out.seconds == 0);
    assert(out.micros == 0);

    ntp.seconds = AMINTP_NTP_TO_AMIGA_EPOCH + 1234;
    ntp.fraction = 0x80000000UL;
    assert(amintp_ntp_to_amiga_time(&ntp, &out) == AMINTP_TIME_OK);
    assert(out.seconds == 1234);
    assert(out.micros == 500000);

    ntp.seconds = AMINTP_NTP_TO_AMIGA_EPOCH - 1;
    ntp.fraction = 0;
    assert(amintp_ntp_to_amiga_time(&ntp, &out) == AMINTP_TIME_ERR_PRE_EPOCH);

    assert(amintp_ntp_to_amiga_time(0, &out) == AMINTP_TIME_ERR_RANGE);
    assert(amintp_ntp_to_amiga_time(&ntp, 0) == AMINTP_TIME_ERR_RANGE);

    puts("M2.1 time conversion tests: PASS");
    return 0;
}
