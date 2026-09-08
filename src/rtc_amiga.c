#include <exec/libraries.h>
#include <proto/battclock.h>
#include <proto/exec.h>
#include <resources/battclock.h>

#include "amintp/rtc.h"

struct Library *BattClockBase;

int amintp_set_rtc_time(const struct amintp_amiga_time *time)
{
    if (time == 0) {
        return 20;
    }

    BattClockBase = OpenResource(BATTCLOCKNAME);
    if (BattClockBase == 0) {
        return 5;
    }

    WriteBattClock((ULONG)time->seconds);
    return 0;
}
