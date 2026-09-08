CC ?= m68k-amigaos-gcc
CFLAGS ?= -Os -Wall -Wextra -Werror -m68000
CPPFLAGS ?= -Iinclude
LDFLAGS ?=
LDLIBS ?=

HOST_CC ?= cc
HOST_CFLAGS ?= -O2 -Wall -Wextra -Werror

TARGET := AmiNTP
COMMON_SOURCES := src/main.c src/cli.c src/version.c src/sntp.c src/query.c
AMIGA_SOURCES := $(COMMON_SOURCES) src/net_amiga.c
OBJECTS := $(AMIGA_SOURCES:.c=.o)
HOST_TARGET := build/host/AmiNTP
M1_TEST := build/host/test_sntp

.PHONY: all clean check host-check m1-check

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

src/%.o: src/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

check:
	@grep -q -- '-m68000' Makefile
	@grep -q 'AMINTP_MIN_OS_MAJOR 2' include/amintp/platform.h
	@grep -q 'AMINTP_MIN_OS_MINOR 4' include/amintp/platform.h
	@grep -q 'AMINTP_AREXX_PORT "AMINTP"' include/amintp/platform.h
	@echo "M0 static checks: PASS"

host-check:
	@mkdir -p build/host
	$(HOST_CC) $(CPPFLAGS) $(HOST_CFLAGS) -o $(HOST_TARGET) $(COMMON_SOURCES) src/net_posix.c
	@tests/m0_host_smoke.sh $(HOST_TARGET)

m1-check:
	@mkdir -p build/host
	$(HOST_CC) $(CPPFLAGS) $(HOST_CFLAGS) -o $(M1_TEST) tests/test_sntp.c src/sntp.c
	@$(M1_TEST)
	$(HOST_CC) $(CPPFLAGS) $(HOST_CFLAGS) -o $(HOST_TARGET) $(COMMON_SOURCES) src/net_posix.c
	@echo "M1.2 host compile: PASS"

clean:
	rm -f $(TARGET) $(OBJECTS)
	rm -rf build
