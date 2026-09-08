# M2.2 qualification

## Scope

M2.2 adds system-clock synchronization while keeping RTC changes out of scope.

## Behavior

- `QUERY` fetches and reports time without changing the clock.
- `SYNC` fetches, validates and converts the NTP timestamp, then updates the Amiga system clock.
- `QUERY` and `SYNC` are mutually exclusive.
- RTC is explicitly unchanged in M2.2.
- Network, SNTP and time-conversion failures must prevent clock changes.

## Host qualification

Run:

```sh
make m2.2-check
```

This runs all prior M0/M1/M2.1 gates plus CLI tests for QUERY/SYNC selection and a host compile with a non-mutating clock backend.

## Native qualification

With Bebbo GCC available:

```sh
make native-check
```

The build must target `-m68000` and include the Amiga clock backend.

## Runtime qualification

In FS-UAE or native hardware with a working bsdsocket.library-compatible stack:

1. Record the current system time.
2. Run `AmiNTP SERVER=<server> QUERY` and verify the system clock is unchanged.
3. Deliberately set the Amiga system clock wrong.
4. Run `AmiNTP SERVER=<server> SYNC`.
5. Verify the system time is corrected.
6. Reboot or inspect RTC separately and verify RTC has not been modified by AmiNTP M2.2.
7. Repeat with an invalid hostname and an unreachable server; verify the clock is not changed.

## Exit criteria

M2.2 is qualified when host gates pass, native `-m68000` build passes, and runtime confirms SYNC changes only the system clock after a valid SNTP reply.
