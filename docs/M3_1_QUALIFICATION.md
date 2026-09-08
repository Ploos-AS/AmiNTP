# M3.1 qualification — ARexx foundation

## Scope

M3.1 introduces the first ARexx host surface for AmiNTP.

The public port name is `AMINTP`.

Supported commands in M3.1:

- `PING`
- `VERSION`
- `STATUS`

`QUERY`, `SYNC`, `LASTSYNC`, `LASTERROR`, and `QUIT` are intentionally deferred to M3.2.

## Resident mode

Start AmiNTP as a resident ARexx host with:

```text
Run >NIL: AmiNTP RESIDENT
```

The process creates public Exec message port `AMINTP` and waits for ARexx commands. In M3.1 it exits on Ctrl-C; `QUIT` arrives in M3.2.

`RESIDENT` is mutually exclusive with CLI `QUERY`, `SYNC`, and `NORTC`.

## ARexx results

M3.1 uses both ARexx RC and RESULT.

Expected examples:

```text
PING    -> RC=0  RESULT=PONG
VERSION -> RC=0  RESULT=AmiNTP 0.3.1-m3.1
STATUS  -> RC=0  RESULT=OK MODE=RESIDENT PORT=AMINTP
NOPE    -> RC=10 RESULT=ERROR CODE=UNKNOWN_COMMAND COMMAND=NOPE
```

An empty command returns RC 20.

## Host qualification

Run:

```sh
make m3.1-check
```

This verifies the platform-independent command dispatcher and confirms the full host build still compiles.

Expected lines include:

```text
M3.1 ARexx dispatcher tests: PASS
M3.1 ARexx host compile: PASS
```

## Native qualification

The Amiga transport uses `rexxsyslib.library` V36 functions and a public Exec message port. Run:

```sh
make native-check
```

with Bebbo GCC available.

Native compilation and runtime behavior remain pending until the consolidated M3.3 FS-UAE qualification. In particular, exact Bebbo/NDK prototype compatibility for `RexxSysBase`, `CreateArgstring`, `IsRexxMsg`, and the public message-port plumbing must not be considered PASS until that native gate is executed.

## Runtime smoke for M3.3

With RexxMast running and AmiNTP resident:

```rexx
ADDRESS AMINTP 'PING'
SAY RC RESULT
ADDRESS AMINTP 'VERSION'
SAY RC RESULT
ADDRESS AMINTP 'STATUS'
SAY RC RESULT
```

Expected results are the values documented above.
