CC ?= m68k-amigaos-gcc
CFLAGS ?= -Os -Wall -Wextra -Werror -m68000
CPPFLAGS ?= -Iinclude
LDFLAGS ?=
LDLIBS ?=

HOST_CC ?= cc
HOST_CFLAGS ?= -O2 -Wall -Wextra -Werror

TARGET := AmiNTP
COMMON_SOURCES := src/main.c src/cli.c src/version.c src/sntp.c src/query.c src/time.c
AMIGA_SOURCES := $(COMMON_SOURCES) src/net_amiga.c src/clock_amiga.c
OBJECTS := $(AMIGA_SOURCES:.c=.o)
HOST_TARGET := build/host/AmiNTP
M1_TEST := build/host/test_sntp
M13_TEST := build/host/test_query
M21_TEST := build/host/test_time
M22_TEST := build/host/test_cli

.PHONY: all clean check host-check m1-check m1.3-check m2.1-check m2.2-check native-check

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
	$(HOST_CC) $(CPPFLAGS) $(HOST_CFLAGS) -o $(HOST_TARGET) $(COMMON_SOURCES) src/net_posix.c src/clock_host.c
	@tests/m0_host_smoke.sh $(HOST_TARGET)

m1-check:
	@mkdir -p build/host
	$(HOST_CC) $(CPPFLAGS) $(HOST_CFLAGS) -o $(M1_TEST) tests/test_sntp.c src/sntp.c
	@$(M1_TEST)
	$(HOST_CC) $(CPPFLAGS) $(HOST_CFLAGS) -o $(HOST_TARGET) $(COMMON_SOURCES) src/net_posix.c src/clock_host.c
	@echo "M1.2 host compile: PASS"

m1.3-check: m1-check
	@mkdir -p build/host
	$(HOST_CC) $(CPPFLAGS) $(HOST_CFLAGS) -o $(M13_TEST) tests/test_query.c src/query.c src/sntp.c
	@$(M13_TEST)
	@echo "M1.3 hardening tests: PASS"

m2.1-check: m1.3-check
	@mkdir -p build/host
	$(HOST_CC) $(CPPFLAGS) $(HOST_CFLAGS) -o $(M21_TEST) tests/test_time.c src/time.c
	@$(M21_TEST)

m2.2-check: m2.1-check
	@mkdir -p build/host
	$(HOST_CC) $(CPPFLAGS) $(HOST_CFLAGS) -o $(M22_TEST) tests/test_cli.c src/cli.c
	@$(M22_TEST)
	$(HOST_CC) $(CPPFLAGS) $(HOST_CFLAGS) -o $(HOST_TARGET) $(COMMON_SOURCES) src/net_posix.c src/clock_host.c
	@echo "M2.2 host compile: PASS"

native-check:
	@command -v $(CC) >/dev/null 2>&1 || { echo "ERROR: $(CC) not found"; exit 1; }
	$(MAKE) clean
	$(MAKE) all
	@file $(TARGET)
	@echo "Native Bebbo build completed"

clean:
	rm -f $(TARGET) $(OBJECTS)
	rm -rf build
