#include "amintp/rtc.h"

int amintp_set_rtc_time(const struct amintp_amiga_time *time)
{
    return time != 0 ? 0 : 20;
}
