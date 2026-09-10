#ifndef AMINTP_TIME_SOURCE_H
#define AMINTP_TIME_SOURCE_H
#include <stdint.h>
struct amintp_wallclock { uint32_t amiga_seconds; uint32_t microseconds; };
int amintp_time_now(struct amintp_wallclock *out);
#endif
