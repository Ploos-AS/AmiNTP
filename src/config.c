#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "amintp/config.h"

static char *trim(char *s)
{
    char *end;
    while (*s != '\0' && isspace((unsigned char)*s)) ++s;
    end = s + strlen(s);
    while (end > s && isspace((unsigned char)end[-1])) --end;
    *end = '\0';
    return s;
}

static int parse_unsigned(const char *text, unsigned long max_value, unsigned long *out)
{
    char *end = 0;
    unsigned long value;
    if (text == 0 || *text == '\0') return 20;
    value = strtoul(text, &end, 10);
    if (end == text || *end != '\0' || value > max_value) return 20;
    *out = value;
    return 0;
}

static int set_server_if_unset(struct amintp_options *options, const char *value)
{
    size_t n;
    if (options->server_set) return 0;
    n = strlen(value);
    if (n == 0 || n > AMINTP_SERVER_MAX) return 20;
    memcpy(options->server_storage, value, n + 1);
    options->server = options->server_storage;
    return 0;
}

int amintp_load_config(const char *path, struct amintp_options *options)
{
    FILE *fp;
    char line[256];
    unsigned line_no = 0;

    errno = 0;
    fp = fopen(path, "r");
    if (fp == 0) {
        if (errno == ENOENT) return 0;
        fprintf(stderr, "AmiNTP: cannot read config %s\n", path);
        return 20;
    }

    while (fgets(line, sizeof(line), fp) != 0) {
        char *text;
        char *eq;
        char *key;
        char *value;
        unsigned long number;
        ++line_no;

        if (strchr(line, '\n') == 0 && !feof(fp)) {
            fprintf(stderr, "AmiNTP: config line %u too long\n", line_no);
            fclose(fp);
            return 20;
        }

        text = trim(line);
        if (*text == '\0' || *text == '#' || *text == ';') continue;
        eq = strchr(text, '=');
        if (eq == 0) {
            fprintf(stderr, "AmiNTP: invalid config line %u\n", line_no);
            fclose(fp);
            return 20;
        }
        *eq = '\0';
        key = trim(text);
        value = trim(eq + 1);

        if (!strcmp(key, "SERVER")) {
            if (set_server_if_unset(options, value) != 0) goto invalid_value;
        } else if (!strcmp(key, "PORT")) {
            if (!options->port_set) {
                if (parse_unsigned(value, 65535UL, &number) != 0 || number == 0) goto invalid_value;
                options->port = (unsigned short)number;
            }
        } else if (!strcmp(key, "TIMEOUT")) {
            if (!options->timeout_set) {
                if (parse_unsigned(value, 3600UL, &number) != 0 || number == 0) goto invalid_value;
                options->timeout_seconds = (unsigned)number;
            }
        } else if (!strcmp(key, "RETRIES")) {
            if (!options->retries_set) {
                if (parse_unsigned(value, 20UL, &number) != 0) goto invalid_value;
                options->retries = (unsigned)number;
            }
        } else {
            fprintf(stderr, "AmiNTP: unknown config key on line %u: %s\n", line_no, key);
            fclose(fp);
            return 20;
        }
    }

    if (ferror(fp)) {
        fprintf(stderr, "AmiNTP: error reading config %s\n", path);
        fclose(fp);
        return 20;
    }
    fclose(fp);
    return 0;

invalid_value:
    fprintf(stderr, "AmiNTP: invalid config value on line %u\n", line_no);
    fclose(fp);
    return 20;
}
