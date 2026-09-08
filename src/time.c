#include "amintp/time.h"

int amintp_ntp_to_amiga_time(const struct amintp_ntp_timestamp *ntp,
                             struct amintp_amiga_time *out)
{
    uint32_t seconds;
    uint32_t micros;

    if (ntp == 0 || out == 0) {
        return AMINTP_TIME_ERR_RANGE;
    }

    if (ntp->seconds < AMINTP_NTP_TO_AMIGA_EPOCH) {
        return AMINTP_TIME_ERR_PRE_EPOCH;
    }

    seconds = ntp->seconds - AMINTP_NTP_TO_AMIGA_EPOCH;
    micros = (uint32_t)(((uint64_t)ntp->fraction * 1000000ULL) >> 32);

    out->seconds = seconds;
    out->micros = micros;
    return AMINTP_TIME_OK;
}

const char *amintp_time_status_string(int status)
{
    switch (status) {
    case AMINTP_TIME_OK:
        return "OK";
    case AMINTP_TIME_ERR_PRE_EPOCH:
        return "PRE_AMIGA_EPOCH";
    case AMINTP_TIME_ERR_RANGE:
        return "RANGE";
    default:
        return "UNKNOWN";
    }
}
