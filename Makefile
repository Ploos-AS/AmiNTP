CC ?= m68k-amigaos-gcc
CFLAGS ?= -Os -Wall -Wextra -Werror -m68000
CPPFLAGS ?= -Iinclude
LDFLAGS ?=
LDLIBS ?=

HOST_CC ?= cc
HOST_CFLAGS ?= -O2 -Wall -Wextra -Werror

TARGET := AmiNTP
COMMON_SOURCES := src/main.c src/cli.c src/config.c src/version.c src/sntp.c src/ipv4.c src/query.c src/time.c src/sync.c src/arexx_core.c src/arexx_ops.c
AMIGA_SOURCES := $(COMMON_SOURCES) src/net_amiga.c src/clock_amiga.c src/rtc_amiga.c src/arexx_amiga.c src/time_source_amiga.c
OBJECTS := $(AMIGA_SOURCES:.c=.o)
HOST_TARGET := build/host/AmiNTP
M1_TEST := build/host/test_sntp
M13_TEST := build/host/test_query
M21_TEST := build/host/test_time
M22_TEST := build/host/test_cli
M23_TEST := build/host/test_sync
M31_TEST := build/host/test_arexx
M32_TEST := build/host/test_arexx_m32
M41_TEST := build/host/test_config

.PHONY: all clean check host-check m1-check m1.3-check m2.1-check m2.2-check m2.3-check m3.1-check m3.2-check m3.3a-check m3.3b-check m3.3c-check m4.1-check m4.2-check native-check

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
	$(HOST_CC) $(CPPFLAGS) $(HOST_CFLAGS) -o $(HOST_TARGET) $(COMMON_SOURCES) src/net_posix.c src/clock_host.c src/rtc_host.c src/arexx_host.c src/time_source_host.c
	@tests/m0_host_smoke.sh $(HOST_TARGET)

m1-check:
	@mkdir -p build/host
	$(HOST_CC) $(CPPFLAGS) $(HOST_CFLAGS) -o $(M1_TEST) tests/test_sntp.c src/sntp.c src/ipv4.c
	@$(M1_TEST)
	$(HOST_CC) $(CPPFLAGS) $(HOST_CFLAGS) -o build/host/test_ipv4 tests/test_ipv4.c src/ipv4.c
	@build/host/test_ipv4
	@echo "M1.0 IPv4 literal tests: PASS"
	$(HOST_CC) $(CPPFLAGS) $(HOST_CFLAGS) -o $(HOST_TARGET) $(COMMON_SOURCES) src/net_posix.c src/clock_host.c src/rtc_host.c src/arexx_host.c src/time_source_host.c
	@echo "M1.2 host compile: PASS"

m1.3-check: m1-check
	@mkdir -p build/host
	$(HOST_CC) $(CPPFLAGS) $(HOST_CFLAGS) -o $(M13_TEST) tests/test_query.c src/query.c src/sntp.c src/ipv4.c src/time_source_host.c
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
	@echo "M2.2 host compile: PASS"

m2.3-check: m2.2-check
	@mkdir -p build/host
	$(HOST_CC) $(CPPFLAGS) $(HOST_CFLAGS) -o $(M23_TEST) tests/test_sync.c src/sync.c
	@$(M23_TEST)
	@echo "M2.3 RTC tests: PASS"

m3.1-check: m2.3-check
	@echo "M3.1 dispatcher foundation covered by M3.2 superset"

m3.2-check: m3.1-check
	@mkdir -p build/host
	$(HOST_CC) $(CPPFLAGS) $(HOST_CFLAGS) -o $(M32_TEST) tests/test_arexx_m32.c src/arexx_core.c src/version.c
	@$(M32_TEST)
	$(HOST_CC) $(CPPFLAGS) $(HOST_CFLAGS) -o $(HOST_TARGET) $(COMMON_SOURCES) src/net_posix.c src/clock_host.c src/rtc_host.c src/arexx_host.c src/time_source_host.c
	@echo "M3.2 ARexx host compile: PASS"

m3.3a-check:
	@bash -n ci/fs-uae/run-aros-smoke.sh
	@bash -n ci/fs-uae/fetch-aros-boot.sh
	@bash -n ci/fs-uae/run-aros-media-smoke.sh
	@grep -q '^kickstart_file = internal$$' ci/fs-uae/aros-smoke.fs-uae
	@grep -q '^kickstart_file = internal$$' ci/fs-uae/aros-media.fs-uae
	@grep -q '^floppy_drive_0 = @AROS_BOOT_ADF@$$' ci/fs-uae/aros-media.fs-uae
	@grep -q 'amiga-m68k-boot-floppy' ci/fs-uae/fetch-aros-boot.sh
	@grep -q 'GATE=FS_UAE_AROS_BOOT_SMOKE' ci/fs-uae/run-aros-smoke.sh
	@grep -q 'GATE=FS_UAE_AROS_BOOT_MEDIA' ci/fs-uae/run-aros-media-smoke.sh
	@echo "M3.3a FS-UAE/AROS harness static checks: PASS"

m3.3b-check:
	@bash -n ci/fs-uae/build-native.sh
	@grep -q 'amigadev/m68k-amigaos-gcc' ci/fs-uae/build-native.sh
	@grep -q 'm68k-amigaos-gcc' ci/fs-uae/build-native.sh
	@grep -q 'GATE=M3_3B_NATIVE_BEBBO_BUILD' ci/fs-uae/build-native.sh
	@grep -q 'Gate 3 - native Bebbo AmiNTP build' .github/workflows/fs-uae-aros.yml
	@echo "M3.3b native-build harness static checks: PASS"

m3.3c-check:
	@bash -n ci/fs-uae/fetch-aros-system.sh
	@bash -n ci/fs-uae/run-aros-guest-smoke.sh
	@grep -q 'amiga-m68k-boot-iso' ci/fs-uae/fetch-aros-system.sh
	@grep -q '^hard_drive_0 = @AROS_ROOT@$$' ci/fs-uae/aros-guest.fs-uae
	@grep -q 'GATE=M3_3C_AROS_GUEST_EXECUTION' ci/fs-uae/run-aros-guest-smoke.sh
	@grep -q 'Gate 4 - execute AmiNTP inside AROS guest' .github/workflows/fs-uae-aros.yml
	@echo "M3.3c guest-execution harness static checks: PASS"

m4.1-check: m3.2-check
	@mkdir -p build/host
	$(HOST_CC) $(CPPFLAGS) $(HOST_CFLAGS) -o $(M41_TEST) tests/test_config.c src/config.c src/cli.c
	@$(M41_TEST)
	@grep -q 'AMINTP_CONFIG_PATH "ENVARC:AmiNTP/AmiNTP.conf"' include/amintp/config.h
	@grep -q '^SERVER=' examples/AmiNTP.conf
	@echo "M4.1 config/startup qualification: PASS"

native-check:
	@command -v $(CC) >/dev/null 2>&1 || { echo "ERROR: $(CC) not found"; exit 1; }
	$(MAKE) clean
	$(MAKE) all
	@file $(TARGET)
	@echo "Native Bebbo build completed"

clean:
	rm -f $(TARGET) $(OBJECTS)
	rm -rf build

# Adversarial host datagrams are separate from real AmiTCP qualification.
m4.2-check: m4.1-check
	@python3 tests/test_net.py $(HOST_TARGET)
