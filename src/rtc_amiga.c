#include <exec/libraries.h>
#define BattClockBase AmiNTPBattClockBase
#include <proto/battclock.h>
#include <proto/exec.h>
#include <resources/battclock.h>

#include "amintp/rtc.h"

struct Library *AmiNTPBattClockBase;

int amintp_set_rtc_time(const struct amintp_amiga_time *time)
{
    if (time == 0) {
        return 20;
    }

    BattClockBase = OpenResource((CONST_STRPTR)BATTCLOCKNAME);
    if (BattClockBase == 0) {
        return 5;
    }

    WriteBattClock((ULONG)time->seconds);
    return 0;
}
