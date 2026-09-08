# M3.4 local AmigaOS ARexx qualification

Overall result: **PASS**, qualified locally on 2026-09-08. All six commands
were executed by the installed AmigaOS RexxMast against AmiNTP's real public
`AMINTP` port. No AROS runtime, mock, or substituted ARexx implementation was used.

## Revision and environment

- Starting HEAD: `555df577698ca9ff28d868f0fde3599d9ed79dbe`.
- Initial `git fetch origin`, `git checkout main`, and
  `git pull --ff-only origin main` succeeded; starting worktree was clean.
- Final HEAD: the atomic commit containing this report, the two-operator harness
  correction, and the evidence directory. Resolve its full SHA with
  `git log -1 --format=%H -- docs/M3_4_QUALIFICATION.md` at this qualification
  revision. A commit cannot embed its own hash; the full final SHA and origin
  verification are recorded in the accompanying completion response.
- Native application sources are unchanged from starting HEAD. The final harness
  is the supplied harness with both unsupported `<>` operators corrected to `~=`.
- FS-UAE: `3.2.35`, existing local `/usr/bin/fs-uae` installation.
- Profile: A1200/020, derived from the existing local A1200 profile;
  emulated CPU 68020, 2 MiB chip RAM, 8 MiB fast RAM, FPU disabled.
- AmigaOS 3.1: guest `Version` reports `Kickstart 40.68, Workbench 40.42`;
  `exec.library 40.10`, `dos.library 40.3`, `rexxsyslib.library 36.23`.
- Existing local A1200 Kickstart 3.1 ROM and Workbench directory were used.
  Workbench was mounted read-only as DH1. A separate generated DH0 contained
  only the native application, qualification scripts and output. No proprietary
  ROM or system files were modified, copied into the repository, or redistributed.
- RexxMast: installed `SYS:System/RexxMast`, banner `ARexx Version 1.15`;
  public `REXX` port observed throughout the test.

## Native build and CLI

Local Bebbo compiler: `/opt/amiga/bin/m68k-amigaos-gcc`,
`m68k-amigaos-gcc (GCC) 6.5.0b 20260807212032`.

```sh
make -B all CC=/opt/amiga/bin/m68k-amigaos-gcc \
  CFLAGS='-Os -Wall -Wextra -Werror -m68000 -mcrt=nix20' \
  LDFLAGS=-mcrt=nix20
```

Build result: PASS, exit 0. `file` identifies an AmigaOS loadseg()ble executable.
SHA-256 of both the built binary and the guest copy:
`535b84646c53c1dabfc94fffa586962f9fa89fe2a334979a091b18215c8c23a4`.
An initial build invocation supplied `-mcrt=nix20` only at link time and failed
on `__locale_ctype_ptr`; rebuilding every object with the runtime selection
at compile time as well resolved the invocation error without a source change.
The native binary was rebuilt after the harness correction.

In the same guest, `Q:AmiNTP VERSION` printed `AmiNTP 0.3.2-m3.2` and
the immediately captured AmigaDOS `$RC` was `0`.

## Actual ARexx results

The startup script launched `Q:AmiNTP RESIDENT` through a background
`Run C:Execute Q:resident`, then invoked the supplied harness through
`SYS:Rexxc/RX Q:m3.4-arexx.rexx`. `OPTIONS RESULTS` requested reply strings.

| Command | Observed RC | Observed RESULT |
| --- | --- | --- |
| PING | 0 | `PONG` |
| VERSION | 0 | `AmiNTP 0.3.2-m3.2` |
| STATUS | 0 | `OK MODE=RESIDENT PORT=AMINTP` |
| LASTSYNC | 0 | `NONE` |
| LASTERROR | 0 | `NONE` |
| QUIT | 0 | `OK QUIT` |

Harness completion:

```text
STATUS=PASS
GATE=M3_4_LOCAL_AMIGAOS_AREXX
```

The immediately captured RX invocation RC was `0`. Separate RX scripts used
`SHOW('P','REXX')` and `SHOW('P','AMINTP')` to inspect actual public ports:

| Observation | REXX | AMINTP |
| --- | --- | --- |
| Before resident launch | 1 | 0 |
| Resident running, before harness | 1 | 1 |
| Two seconds after harness/QUIT | 1 | 0 |

The background wrapper also captured resident AmiNTP returning RC `0`.
Thus QUIT both received its successful reply and terminated the resident
instance, removing AMINTP while RexxMast remained active.

## Defect found and rerun

The unmodified harness genuinely executed PING (`0`, `PONG`) but then failed
with `Error 41 in line 35: Invalid expression`, RX RC 10. AMINTP remained
present because the harness never reached QUIT. That initial run is **FAIL**.

ARexx supports `~=` for inequality, not the harness's `<>` spelling; see the
[AmigaOS ARexx operator reference](https://wiki.amigaos.net/wiki/AmigaOS_Manual%3A_ARexx_Elements_of_ARexx).
Both occurrences were corrected. No expected value, RC check, command, or
success condition was relaxed. The complete six-command harness, CLI sanity
check, and port lifecycle checks then ran again from a fresh emulator boot.

## Complete host/static regression

After the fix and native rebuild, this command exited 0:

```sh
make check host-check m3.2-check m3.3a-check m3.3b-check m3.3c-check
```

PASS: M0 platform checks and CLI smoke; M1 SNTP wire and host compile;
M1.3 query hardening; M2.1 time conversion; M2.2 CLI; M2.3 RTC orchestration;
M3.2 ARexx dispatcher (the Makefile's M3.1 superset); all M3.3a/b/c harness
static checks. This covers all existing host/static qualification targets.
The AROS harnesses were checked statically only, not used as ARexx evidence.
`git diff --check` also passed.

## Evidence and limitations

[Evidence directory](evidence/m3.4/) includes raw initial and final guest
outputs, native build and host/static logs, the exact qualification startup
and port scripts, local FS-UAE configuration, and emulator configuration
excerpts confirming CPU, ROM identification, and read-only Workbench mapping.
The configuration contains local absolute paths and must be adapted on another
machine. FS-UAE's documented
[read-only drive option](https://fs-uae.net/docs/options/hard-drive-0-read-only/)
was confirmed in the emulator log as `ro,DH1:Workbench`.
Full local emulator logs and the native binary remain under ignored `build/m3.4/`.
The successful emulator run was stopped by a 35-second host timeout (exit 124)
after the guest wrote `DONE`; this is not the guest harness or application RC.

Qualification is limited to this AmigaOS 3.1/68020 profile running a 68000-target
binary. Real 68000 hardware and AmigaOS 2.04 were not exercised. QUERY, SYNC,
SERVER, TCP/IP integration, clock/RTC changes, concurrent clients and error
reply paths remain outside this six-command gate. No blockers remain for M3.4.
