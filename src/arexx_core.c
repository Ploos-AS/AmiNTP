#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "amintp/arexx.h"
#include "amintp/platform.h"
#include "amintp/version.h"

static void normalize_command(const char *src, char *dst, size_t dst_size)
{
    size_t n = 0;

    if (dst_size == 0) return;
    while (src != 0 && *src != '\0' && isspace((unsigned char)*src)) ++src;
    while (src != 0 && *src != '\0' && !isspace((unsigned char)*src) && n + 1 < dst_size) {
        dst[n++] = (char)toupper((unsigned char)*src++);
    }
    dst[n] = '\0';
}

int amintp_arexx_dispatch(const char *command, char *result, size_t result_size)
{
    char verb[32];

    if (result == 0 || result_size == 0) return 20;
    result[0] = '\0';
    normalize_command(command, verb, sizeof(verb));

    if (!strcmp(verb, "PING")) {
        snprintf(result, result_size, "PONG");
        return 0;
    }
    if (!strcmp(verb, "VERSION")) {
        snprintf(result, result_size, "%s", amintp_version_string());
        return 0;
    }
    if (!strcmp(verb, "STATUS")) {
        snprintf(result, result_size, "OK MODE=RESIDENT PORT=%s", AMINTP_AREXX_PORT);
        return 0;
    }
    if (verb[0] == '\0') {
        snprintf(result, result_size, "ERROR CODE=EMPTY_COMMAND");
        return 20;
    }

    snprintf(result, result_size, "ERROR CODE=UNKNOWN_COMMAND COMMAND=%s", verb);
    return 10;
}
