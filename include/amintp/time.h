#ifndef AMINTP_TIME_H
#define AMINTP_TIME_H

#include <stdint.h>
#include "amintp/sntp.h"

#define AMINTP_NTP_TO_AMIGA_EPOCH 2461449600UL

enum amintp_time_status {
    AMINTP_TIME_OK = 0,
    AMINTP_TIME_ERR_PRE_EPOCH = -1,
    AMINTP_TIME_ERR_RANGE = -2
};

struct amintp_amiga_time {
    uint32_t seconds;
    uint32_t micros;
};

int amintp_ntp_to_amiga_time(const struct amintp_ntp_timestamp *ntp,
                             struct amintp_amiga_time *out);
const char *amintp_time_status_string(int status);

#endif
