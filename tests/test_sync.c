#include <assert.h>
#include <stdio.h>

#include "amintp/clock.h"
#include "amintp/rtc.h"
#include "amintp/sync.h"

static int clock_rc;
static int rtc_rc;
static int order;
static int clock_order;
static int rtc_order;

int amintp_set_system_time(const struct amintp_amiga_time *time)
{
    assert(time != 0);
    clock_order = ++order;
    return clock_rc;
}

int amintp_set_rtc_time(const struct amintp_amiga_time *time)
{
    assert(time != 0);
    rtc_order = ++order;
    return rtc_rc;
}

static void reset_state(void)
{
    clock_rc = 0;
    rtc_rc = 0;
    order = 0;
    clock_order = 0;
    rtc_order = 0;
}

int main(void)
{
    struct amintp_amiga_time t = {1234, 500000};

    reset_state();
    assert(amintp_apply_time(&t, 1) == 0);
    assert(clock_order == 1 && rtc_order == 2);

    reset_state();
    assert(amintp_apply_time(&t, 0) == 0);
    assert(clock_order == 1 && rtc_order == 0);

    reset_state();
    clock_rc = 10;
    assert(amintp_apply_time(&t, 1) == 10);
    assert(clock_order == 1 && rtc_order == 0);

    reset_state();
    rtc_rc = 5;
    assert(amintp_apply_time(&t, 1) == 5);
    assert(clock_order == 1 && rtc_order == 2);

    puts("M2.3 RTC orchestration tests: PASS");
    return 0;
}
