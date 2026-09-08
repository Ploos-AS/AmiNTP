# M1.3 qualification and hardening

M1.3 closes the first SNTP networking milestone with deterministic host tests and an explicit native/runtime gate.

## Automated host qualification

Run:

```sh
make check
make host-check
make m1.3-check
```

The M1.3 query tests use a mocked UDP transport and verify:

- a structurally valid SNTP server reply succeeds;
- transport failure (the class used for DNS/socket/timeout failure) returns RC 10;
- a transport-successful but invalid NTP reply returns RC 10.

No Internet access is required for these tests.

## Native Bebbo build gate

With Bebbo GCC in PATH:

```sh
make native-check
```

This must compile the Amiga transport, including `src/net_amiga.c`, using `-m68000` and produce the native `AmiNTP` executable.

The native gate is authoritative for Amiga headers/ABI and cannot be replaced by the POSIX CI build.

## Runtime QUERY gate

Run on a classic Amiga or FS-UAE configuration with a bsdsocket.library-compatible TCP/IP stack:

```text
AmiNTP SERVER=<working-ntp-server> QUERY TIMEOUT=5 RETRIES=2
```

Expected: RC 0 and an `OK SERVER=... STRATUM=... NTP_SECONDS=... NTP_FRACTION=...` line.

Then verify failure behavior:

```text
AmiNTP SERVER=no-such-host.invalid QUERY TIMEOUT=2 RETRIES=0
```

Expected: RC 10, no clock modification, and no hang.

For timeout qualification, use a known non-responsive IPv4 destination on the test network and verify that the command returns after the configured bounded timeout/retry window with RC 10.

## M1.3 exit criteria

1. M0 gates remain green.
2. SNTP wire tests pass.
3. deterministic query success/transport-failure/protocol-failure tests pass.
4. Bebbo native build succeeds with `-m68000`.
5. runtime QUERY succeeds through `bsdsocket.library`.
6. DNS and timeout failures terminate cleanly with RC 10.
7. QUERY never changes the system clock.

Items 4-6 require the native toolchain/runtime environment and must be recorded when executed.
