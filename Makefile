CC ?= m68k-amigaos-gcc
CFLAGS ?= -Os -Wall -Wextra -Werror -m68000
CPPFLAGS ?= -Iinclude
LDFLAGS ?=
LDLIBS ?=

TARGET := AmiNTP
SOURCES := src/main.c src/cli.c src/version.c
OBJECTS := $(SOURCES:.c=.o)

.PHONY: all clean check

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

clean:
	rm -f $(TARGET) $(OBJECTS)
