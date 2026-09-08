# M2.3 qualification — RTC synchronization

## Scope

M2.3 extends `SYNC` from system-clock-only operation to optional battery-backed RTC synchronization.

Policy:

- `SYNC` always sets the Amiga system clock first.
- By default, `SYNC` then writes the same Amiga-epoch seconds to `battclock.resource`.
- `NORTC` skips the RTC write completely.
- If the system clock write fails, the RTC must not be attempted.
- If the system clock succeeds but the RTC write fails or is unavailable, AmiNTP returns RC 5 because the requested sync was only partially completed.
- `QUERY` never changes either clock.

## Classic Amiga API

The native backend uses `battclock.resource`, available from V36, and `WriteBattClock()`. The value is seconds since 1978-01-01, matching AmiNTP's M2.1 time representation.

## Host qualification

Run:

```sh
make m2.3-check
```

This verifies:

- system clock is written before RTC;
- `NORTC` suppresses the RTC call;
- system-clock failure suppresses the RTC call;
- RTC failure after a successful system update returns RC 5;
- CLI accepts `SYNC NORTC` and rejects `QUERY NORTC`.

Expected final lines include:

```text
M2.3 RTC orchestration tests: PASS
M2.3 RTC tests: PASS
```

## Native qualification

With Bebbo GCC installed:

```sh
make native-check
file AmiNTP
```

The build must remain `-m68000`, no FPU, AmigaOS 2.04+ compatible.

## Runtime qualification

On Amiga/FS-UAE with working networking:

```text
AmiNTP SERVER=<server> SYNC NORTC
```

Expected: system time changes, battery clock does not.

Then:

```text
AmiNTP SERVER=<server> SYNC
```

Expected: system time changes first and RTC is updated afterwards. A successful result reports `RTC=UPDATED`.

Power-cycle or reboot qualification should confirm the battery-backed value survives and is loaded by the OS as expected.

## Exit criteria

M2.3 is code-complete when host gates pass and native build remains valid. Runtime qualification remains pending until tested on an Amiga/FS-UAE environment with a battery clock implementation.
