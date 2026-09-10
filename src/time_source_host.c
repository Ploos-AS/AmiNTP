#ifndef __amigaos__
#include <sys/time.h>
#include "amintp/time_source.h"
int amintp_time_now(struct amintp_wallclock *out){struct timeval t;if(!out||gettimeofday(&t,0)!=0)return 20;out->amiga_seconds=(uint32_t)t.tv_sec-252460800UL;out->microseconds=(uint32_t)t.tv_usec;return 0;}
#endif
