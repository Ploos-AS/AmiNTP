#include "amintp/clock.h"
#include "amintp/rtc.h"
#include "amintp/sync.h"

int amintp_apply_time(const struct amintp_amiga_time *time, int write_rtc)
{
    int rc;

    if (time == 0) {
        return 20;
    }

    rc = amintp_set_system_time(time);
    if (rc != 0) {
        return rc;
    }

    if (!write_rtc) {
        return 0;
    }

    rc = amintp_set_rtc_time(time);
    if (rc != 0) {
        return 5;
    }

    return 0;
}
