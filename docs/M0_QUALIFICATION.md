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

## Automated qualification

The repository has two host-side gates:

```sh
make check
make host-check
```

`make check` validates the frozen platform/build contract. `make host-check`
compiles the portable M0 CLI sources with the host C compiler and verifies
VERSION, HELP, SERVER parsing, and the expected M0 return code.

GitHub Actions runs both gates on pushes to `main`, pull requests, and manual
dispatches.

Expected output includes:

```text
M0 static checks: PASS
M0 host smoke: PASS
```

Host compilation is not a substitute for the native Amiga build gate.

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

## Native CLI smoke qualification

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
5. automated static and host smoke gates pass;
6. native Bebbo build produces an AmigaOS loadseg() executable;
7. native CLI smoke passes on AmigaOS 2.04+.

Until items 6 and 7 have been observed, M0 is code-complete but native
qualification remains pending.
