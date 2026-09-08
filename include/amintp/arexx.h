#ifndef AMINTP_AREXX_H
#define AMINTP_AREXX_H

#include <stddef.h>
#include "amintp/sntp.h"
#include "amintp/time.h"

#define AMINTP_AREXX_RESULT_SIZE 192

int amintp_arexx_dispatch(const char *command, char *result, size_t result_size);
int amintp_arexx_run(void);
int amintp_arexx_should_quit(void);
void amintp_arexx_reset_state(void);

int amintp_arexx_query_server(const char *server,
                              struct amintp_sntp_reply *reply,
                              struct amintp_amiga_time *amiga_time);
int amintp_arexx_sync_server(const char *server, int write_rtc,
                             struct amintp_sntp_reply *reply,
                             struct amintp_amiga_time *amiga_time);

#endif
