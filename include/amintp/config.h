#ifndef AMINTP_CONFIG_H
#define AMINTP_CONFIG_H

#include "amintp/cli.h"

#define AMINTP_CONFIG_PATH "ENVARC:AmiNTP/AmiNTP.conf"

int amintp_load_config(const char *path, struct amintp_options *options);

#endif
