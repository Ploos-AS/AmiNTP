# M2.1 qualification — NTP to Amiga time

## Scope

M2.1 introduces the time model used by later clock-setting code.

Policy:

- NTP timestamps are interpreted as UTC.
- Amiga time is represented as seconds and microseconds since 1978-01-01 00:00:00 UTC.
- No timezone or DST adjustment is performed in the conversion layer.
- No floating point is required.
- System clock and RTC writes are intentionally deferred to M2.2/M2.3.

## Epoch

NTP epoch: 1900-01-01 00:00:00 UTC.

Amiga epoch used by AmiNTP: 1978-01-01 00:00:00 UTC.

The fixed offset is 2461449600 seconds.

## Host qualification

Run:

```sh
make m2.1-check
```

This runs all previous M1 checks and verifies:

- exact Amiga epoch conversion;
- integer NTP fraction to microsecond conversion;
- rejection of pre-1978 timestamps;
- null/range guard behavior.

Expected final line:

```text
M2.1 time conversion tests: PASS
```

## Native qualification

Run with Bebbo GCC available:

```sh
make native-check
```

The full AmiNTP binary must still compile with `-m68000` and no FPU requirement.

## Runtime observation

On Amiga/FS-UAE with a working bsdsocket.library stack:

```text
AmiNTP SERVER=pool.ntp.org QUERY
```

A successful reply includes:

```text
AMIGA_SECONDS=<seconds> AMIGA_MICROS=<microseconds>
```

M2.1 must not modify the system clock or RTC.

## Exit criteria

M2.1 is complete when host conversion tests pass, the native build remains 68000-compatible, and QUERY can expose converted Amiga time without changing the clock.
