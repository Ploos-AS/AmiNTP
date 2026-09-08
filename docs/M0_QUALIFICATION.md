# M0 qualification

## Scope

M0 establishes the AmiNTP repository and freezes the minimum platform contract.

## Required contract

- AmigaOS 2.04+
- Motorola 68000 minimum
- no FPU
- no GUI
- Bebbo GCC-compatible build
- ARexx port reserved as `AMINTP`
- networking intentionally deferred to M1

## Static qualification

Run:

```sh
make check
```

Expected:

```text
M0 static checks: PASS
```

## Native build qualification

With a Bebbo GCC toolchain in PATH:

```sh
make clean
make
file AmiNTP
```

The compile command must contain:

```text
-m68000
```

The resulting executable must be an AmigaOS loadseg()-compatible executable.

## CLI smoke qualification

On an Amiga or emulator:

```text
AmiNTP VERSION
AmiNTP HELP
AmiNTP SERVER=pool.ntp.org
```

Expected M0 behavior:

- VERSION returns success and prints the M0 version.
- HELP returns success and prints usage.
- SERVER is parsed but returns WARN because SNTP is not implemented until M1.

## Return-code baseline

- 0 — success
- 5 — warning / requested operation unavailable or no clock change
- 10 — network/protocol failure
- 20 — configuration/internal/usage failure

## M0 exit criteria

M0 is complete when:

1. repository structure is present;
2. platform contract is documented and encoded in headers;
3. build explicitly targets 68000;
4. CLI skeleton parses help/version/server;
5. static checks pass;
6. native build/runtime qualification is ready to execute with the toolchain.
