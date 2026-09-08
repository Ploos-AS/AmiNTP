#ifndef AMINTP_AREXX_H
#define AMINTP_AREXX_H

#include <stddef.h>

#define AMINTP_AREXX_RESULT_SIZE 160

int amintp_arexx_dispatch(const char *command, char *result, size_t result_size);
int amintp_arexx_run(void);

#endif
