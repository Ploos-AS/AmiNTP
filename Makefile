CC ?= m68k-amigaos-gcc
CFLAGS ?= -Os -Wall -Wextra -Werror -m68000
CPPFLAGS ?= -Iinclude
LDFLAGS ?=
LDLIBS ?=

HOST_CC ?= cc
HOST_CFLAGS ?= -O2 -Wall -Wextra -Werror

TARGET := AmiNTP
SOURCES := src/main.c src/cli.c src/version.c
OBJECTS := $(SOURCES:.c=.o)
HOST_TARGET := build/host/AmiNTP

.PHONY: all clean check host-check

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
	$(HOST_CC) $(CPPFLAGS) $(HOST_CFLAGS) -o $(HOST_TARGET) $(SOURCES)
	@tests/m0_host_smoke.sh $(HOST_TARGET)

clean:
	rm -f $(TARGET) $(OBJECTS)
	rm -rf build
