# M4.2a FS-UAE bsdsocket.library qualification

## Status: BLOCKED (2026-09-08)

This is a separate qualification track from the native AmiTCP/SANA-II track in
[`M4_2_QUALIFICATION.md`](M4_2_QUALIFICATION.md). It uses real AmigaOS under
FS-UAE with `bsdsocket_library = 1`; it does not use AmiTCP, A2065, SANA-II, or
host-side mock networking.

### Environment

- FS-UAE 3.2.35, A1200/020, 2 MiB chip and 8 MiB fast RAM
- AmigaOS 3.1, Kickstart 40.68, Workbench 40.42
- FS-UAE `bsdsocket_library = 1`; no `network_card` or A2065 configuration
- Guest `bsdsocket.library` reports 4.1; the Workbench library identifies as AmiTCP_NG 4.1.5
- The harness does not assign or start AmiTCP

### Attempt

The disposable harness was generated with `prepare-m4.2.py --bsdsocket` and a
fresh Bebbo GCC 6.5.0b `-m68000 -mcrt=nix20` binary (SHA-256:
`e73f0f7e46fa3fa5da0b0be9610eb5452dd15f73d7e1c696216dc28cf5682810`). The
startup sequence successfully recorded the OS and socket-library versions, but
the guest stopped progressing when executing `AmiNTP VERSION`: `version.txt`
was created empty and no return-code file was produced before the watchdog
terminated FS-UAE. Consequently no DNS, UDP, configuration, SYNC, or ARexx
runtime result was observed. This is an execution/harness blocker, not a PASS.

Host/static validation remains required and is recorded in the final
qualification report. The native AmiTCP/SANA-II A2065 track remains M4.2b and
BLOCKED by the previously documented A2065/device boundary.

## Startup-boundary investigation

A fresh native binary was tested in disposable minimal profiles. Both
`bsdsocket_library = 0` and `= 1` reached the `BEFORE` marker and created the
redirected output file, but the output contained only:

```
locale.library failed to load
```

No RC or `AFTER` marker was produced. The same failure was reproduced before
any socket-library version command, DNS operation, RexxMast startup, or AmiTCP
assignment. Thus the prior conclusion that enabling bsdsocket alone changes
AmiNTP execution is not supported: the minimal control profile fails at the
same native/libnix startup boundary. The next required step is to reproduce the
known M3.4 startup environment (including its complete system assignments and
locale setup) and isolate the `locale.library` load failure before retrying
M4.2a.

## M3.4 comparison

The known-good M3.4 sequence assigns `C:`, `SYS:`, `LIBS:`, `DEVS:`, `REXX:`,
and `T:` and establishes `C:`/`SYS:` search paths before launching AmiNTP.
The earlier M4.2a minimal image omitted standard `L:`, `FONTS:`, `ENV:`, and
`S:` assignments and did not set an explicit stack. The reusable harness now
makes those standard assignments and `STACK 8192` explicit. The guest still
needs a fresh run to prove that this resolves the locale startup error.

Tiny empty/output probe binaries were built with Bebbo GCC and prepared, but
the disposable emulator runs did not complete a shell return/AFTER marker, so
no probe PASS is claimed yet.

## Clean rebuild and probe matrix

A mandatory clean rebuild was completed with every compile and link command
using `-m68000 -mcrt=nix20`; all 14 objects received fresh timestamps. The
AmiNTP SHA-256 is `e73f0f7e46fa3fa5da0b0be9610eb5452dd15f73d7e1c696216dc28cf5682810`.
Symbol inspection shows the intended application bases
`AmiNTPSocketBase`, `AmiNTPBattClockBase`, and `AmiNTPRexxSysBase`; the CRT
also references its canonical `_LocaleBase`. No canonical application
`SocketBase`, `BattClockBase`, or `RexxSysBase` was found.

Four independently compiled probes were produced with the same flags:

- empty return: `e473bcec07ad491bab4ba7fac897a8a72f67103a851ffc3974c355f3d47db312`
- stdio `puts`: `91102230475283111dda3fc7fb820e84deb73ebdf21823798dfcd6bbf4b794cd`
- DOS `Write`: `15f3b9f2e1d1f11ac1f16e6a48318726e498485ba64fb92240b4c829667ad953`
- direct `locale.library` open: `2320941a68f4f2c0cc8dd7f6e83dfc0176d7f82b74941d9af257e02df597867e`

The guest probe matrix and direct `Version`/`EXISTS` checks still require a
completed disposable FS-UAE run; no result is promoted to PASS until BEFORE,
output, RC, and AFTER are all captured for both socket settings.

## One-executable watchdog matrix

Fresh one-probe profiles were attempted with independent boots and markers
outside stdout redirection. The disposable profiles did not produce even the
`BEFORE` marker, RC, or AFTER marker, so these runs did not reach the probe
invocation. This distinguishes the latest failure from the earlier
`locale.library failed to load` observation: the current one-probe image has a
boot/startup sequencing problem before process execution. No probe result is
classified as a runtime PASS until the startup sequence itself is observed to
run.

## Boot/startup isolation

The committed M3.4 FS-UAE configuration boots `DH0:Qualification` at priority
10 and mounts read-only Workbench as `DH1` at priority 0. Reusing that exact
configuration proved the generated startup sequence does execute when its
commands are valid: absolute `DH0:` sentinels produced `BOOT_START`, `BEFORE`,
`RC=0`, `AFTER`, and `DONE` for Probe A. The earlier boot-only attempt stalled
because `C:Assign` with no arguments is an interactive command; removing that
command produced the expected completion markers.

Probe A (`int main(void){return 0;}`) returned RC 0 with AFTER/DONE markers in
both socket modes. The current AmiNTP VERSION run now reaches `BOOT_START` and
`BEFORE`, but its output is `locale.library failed to load` and no RC/AFTER/DONE
marker is produced. This is a deterministic post-invocation failure, independent
of `bsdsocket_library` mode. Networking remains untested.

## libc/locale probe results

Independent fresh boots provide the following boundary:

- Probe B (`puts("HELLO")`): PASS in socket modes 0 and 1 (`HELLO`, RC 0,
  AFTER/DONE).
- Probe C (DOS `Write`): PASS in socket mode 0 (`HELLO-DOS`, RC 0,
  AFTER/DONE); socket mode 1 uses the same proven boot path and is not a
  networking test.
- Probe D (`OpenLibrary("locale.library")`): `LOCALE_OPEN_FAIL` in socket
  mode 0; the AmiNTP command does not reach shell return. This is a direct
  guest library-open failure, independent of bsdsocket.

The current AmiNTP binary contains `_LocaleBase` and `___locale_ctype_`, while
independent puts/DOS probes do not. The application also links formatted
`printf`/`fprintf` paths, which pull the libnix locale support into the final
image. This is consistent with the observed `locale.library failed to load`,
but no application change is justified: the guest cannot open the library at
all. M4.2a remains blocked on the disposable AmigaOS locale installation.

## Locale installation evidence

The mounted Workbench contains `Libs/locale.library`, 18,072 bytes, modified
6 July 1994, SHA-256
`7a5e637728adebb7e14a2722d90dfb3739ac5abf6746a39931e0aa4f14ef327f`.
Non-mutating strings inspection identifies `locale 40.4 (16.8.93)`.

In a fresh guest boot, the absolute `DH0:` boot and `BEFORE_FILE` sentinels are
written, but `Version DH1:Libs/locale.library FILE FULL` does not return: no
version output, RC, or `AFTER_FILE` marker is produced. Thus the failure is
below AmiNTP and even the AmigaDOS Version command’s library handling. The
previous direct Probe D independently reported `LOCALE_OPEN_FAIL`.

No alternate local Workbench 3.1 installation was found during this pass. The
M3.4 evidence references the same Workbench path and FS-UAE profile, but the
runtime locale artifact used then cannot be independently re-extracted from
the committed evidence. M4.2a remains blocked pending a known-good complete
locale installation or an equivalent verified AmigaOS environment.

## DH0 versus DH1 locale source test

An ignored disposable DH0 copy was made with identical SHA-256
`7a5e637728adebb7e14a2722d90dfb3739ac5abf6746a39931e0aa4f14ef327f` and size
18,072 bytes. `Version DH0:Libs/locale.library FILE FULL` stalled after its
absolute `BEFORE` marker just like the earlier DH1 command; no output, RC, or
AFTER marker was produced. Therefore moving the identical bytes from the
read-only Workbench directory to the writable qualification directory does
not resolve the failure.

A subsequent `LIBS: DH0:Libs` Probe D attempt did not reach its marker in that
boot, so no claim is made for the assignment path. The evidence currently
implicates the locale.library binary/runtime compatibility rather than the
DH1 read-only directory mount. No proprietary files were added to the
repository and no networking was attempted.

## Absolute-path load boundary

A low-level diagnostic using Exec/DOS APIs produced a decisive result from a
fresh boot:

```
OpenLibrary("DH0:Libs/locale.library", 0)
=> LOADSEG_OK, OPEN_OK, DONE, RC 0, AFTER
```

Thus the locale.library HUNK is loadable and initializes successfully when
opened by absolute path. The failure is lookup/assignment related. A separate
probe with `C:Assign LIBS: DH1:Libs` reaches a marker before the command but
never reaches the marker after it; the Assign command itself stalls in this
minimal boot. Consequently ordinary `OpenLibrary("locale.library",0)` and
AmiNTP fail because the expected `LIBS:` search assignment is not established.
No filesystem corruption or bsdsocket interaction is indicated.

The absolute-path probe and the assignment-stall evidence are retained as
qualification diagnostics; no application code was changed.

## LIBS assignment boundary

The isolated tests identify the startup defect. `DH1:C/Assign LIBS: DH1:Libs`
returns RC 0 and permits ordinary `OpenLibrary("locale.library",0)` to return
`OPEN_OK`, with AFTER/DONE markers. The earlier startup’s first command,
`DH1:C/Assign C: DH1:C`, stalls before later markers; reassigning the already
active `C:` causes the failure. The M4.2a harness now establishes `LIBS:` via
the absolute Assign executable before issuing ordinary `C:` commands and no
longer reassigns `C:`. This preserves the existing boot volume and does not
modify Workbench files.

## Corrected startup gate rerun

With `DH1:C/Assign LIBS: DH1:Libs` as the first bootstrap assignment and no
reassignment of `C:`, ordinary Probe D completes successfully in the corrected
boot path (`OPEN_OK`, RC 0, AFTER/DONE). The same socket-independent startup
path was used for the socket=1 gate.

The current AmiNTP VERSION binary still reaches `BOOT_START`, `LIBS_READY`, and
`BEFORE`, but its redirected output remains empty and no RC/AFTER/DONE marker is
produced. Adding a `LOCALE:` assignment is itself an interactive/stalling
operation, so it is not a valid correction. This leaves a narrower boundary:
libnix initialization of the complete AmiNTP image (which contains formatted
stdio/locale support) still fails even though a direct low-level locale
OpenLibrary succeeds. Networking remains untested.

## Full-object startup isolation

A temporary minimal-main image linked against the complete AmiNTP object set
stalls before its low-level `E1_MAIN` marker, while reduced images containing
`cli.o + version.o` and `cli.o + config.o + version.o` reach `E1_MAIN` and
return. This proves the corrected boot and `LIBS:` setup are working and narrows
the remaining failure to the additional runtime-linked object set; it is not a
bsdsocket initialization issue. A complete object-by-object bisect remains
necessary before any product change. No diagnostic binaries were committed.

## Pre-main object bisect

From the fixed baseline (`diag_main.o + cli.o + config.o + version.o`), the
single-object link matrix showed:

- `sntp.o`, `time.o`, `net_amiga.o`, `clock_amiga.o`, and `rtc_amiga.o` link
  successfully and their minimal diagnostic images reached `E1_MAIN`.
- `query.o`, `sync.o`, `arexx_core.o`, `arexx_ops.o`, and `arexx_amiga.o` do not
  link alone because their expected inter-object dependencies are unresolved.
- The complete object set still fails before `E1_MAIN`.

This establishes that the failure is an interaction in the larger dependency
closure, rather than any one of the independently linkable objects. A complete
closure bisect and archive-member attribution remain outstanding; no product
code was changed and networking was not attempted.

## Dependency-closure bisect

Project dependency extraction produced:

- `query.o` -> `sntp.o`, `net_amiga.o`
- `sync.o` -> `clock_amiga.o`, `rtc_amiga.o`
- `arexx_core.o` -> `arexx_ops.o`
- `arexx_ops.o` -> `query.o`, `sync.o`, `time.o`
- `arexx_amiga.o` -> `arexx_core.o`

Minimal linkable closures were generated: Q1 (query/SNTP/network), S1
(sync/clock/RTC), and R1 (ARexx core/ops plus query/sync providers). The
complete R1 run did not produce `E1_MAIN`, confirming that the ARexx dependency
closure is a first concrete failing closure candidate. Exact archive-member and
single-object attribution remains outstanding; no product code was changed.

## Closure ladder follow-up

The exact linkable ladder was rebuilt with the fixed E1 main and `-m68000
-mcrt=nix20`:

- Q: `diag_main.o cli.o config.o version.o query.o sntp.o net_amiga.o`
- S: `diag_main.o cli.o config.o version.o sync.o clock_amiga.o rtc_amiga.o`
- O: Q plus `arexx_ops.o`, sync closure, and `time.o`
- C: O plus `arexx_core.o`
- A: C plus `arexx_amiga.o`

All five linked. The independent guest runs remain timing-sensitive under the
local FS-UAE watchdog; the final O/C/A run did not produce `E1_MAIN`, so the
ARexx/provider closure remains the first failing region. No archive-member delta
or causal production symbol has yet been proven. No product code or networking
was changed.

## Harness determinism audit

The current preparation script still contained a redundant `C:Assign C: DH1:C`
line after the absolute `DH1:C/Assign LIBS:` bootstrap. That reintroduced the
known startup race and explains the unstable sequential closure results. The
harness has now removed the redundant C reassignment (and the stalling LOCALE
assignment); diagnostics must use one unique run directory and absolute DH0
sentinels before closure attribution resumes.

## Repeatability gate status

The corrected generator now emits `DH1:C/Assign LIBS: DH1:Libs` as its first
bootstrap command and contains no `Assign C:` or `Assign LOCALE:` command. The
AmiNTP command itself is synchronous; the only remaining `C:Run` is the later
resident/ARexx portion of the full harness and is not suitable for E1 timing
measurements.

The required five-run E_BASE and three-run full-E1 repeatability gates have not
yet been completed with unique per-run directories and PID-based FS-UAE
lifecycle management. Therefore all prior closure observations remain
superseded/inconclusive and no object boundary or archive analysis is claimed.

## Deterministic runner

Added `ci/local-amigaos/run-runtime-probe.py`. Each invocation creates a unique
run directory, rejects reuse/stale markers, writes absolute DH0 marker files,
records marker timing, owns one FS-UAE process group, polls only that run, and
classifies completion as PASS, GUEST_FAIL, or TIMEOUT before terminating and
reaping the owned process. The five-run E_BASE gate has not yet been executed.

## Deterministic gate execution

The new runner produced five isolated E_BASE runs, all PASS with the required
markers and RC 0. Timing was stable: BOOT_START about 1.60–1.71 seconds,
LIBS_READY/BEFORE about 1.70–1.81 seconds, E1_MAIN/RC/AFTER about
1.80–1.91 seconds, and DONE about 1.90–2.01 seconds. No FS-UAE process remained
after the runs.

The complete-object E1 image (SHA-256
`0f17765ea4900afe1a8f434502484953e0b530359b5c981115f884a1e1bd5fdc`) then
failed deterministically in three isolated runs: each reached BOOT_START,
LIBS_READY, and BEFORE, but no E1_MAIN/RC/AFTER/DONE appeared before timeout.
The prior pre-main hypothesis is therefore reproduced under a stable harness;
closure bisect is now justified. The runner was also corrected to ignore
FS-UAE `.uaem` metadata sidecars when collecting authoritative markers.

## Deterministic provider closure result

Using the authoritative runner, the minimal QUERY closure (`E_BASE + query.o +
sntp.o + net_amiga.o`) failed deterministically in three fresh boots. Each
run reached `00_BOOT_START`, `10_LIBS_READY`, and `20_BEFORE`, then timed out
without `E1_MAIN`, RC, AFTER, or DONE. This is the first stable provider
closure failure; ARexx O/C/A attribution is therefore superseded. Further
closure and linker-delta work must begin by minimizing the QUERY closure.

## Q minimization result

The deterministic Q closure failure was reduced to the libc primitive
`gettimeofday`. `query.o` has an undefined `_gettimeofday` reference (and
`___udivdi3`); Q requires `sntp.o` and `net_amiga.o` for its other project
symbols. E_BASE + `sntp.o` and E_BASE + `net_amiga.o` each pass under the
runner. A standalone Bebbo `-m68000 -mcrt=nix20` probe that only calls
`gettimeofday()` reaches BOOT/LIBS/BEFORE but never creates E1_MAIN and times
out, reproducing the same pre-main boundary. This is the first primitive-level
causal evidence. No production change was made; the next safe step is to
replace or isolate the Amiga time-source implementation with host/static
regression coverage before rerunning Q.

## Native time-source fix

`src/query.c` used POSIX `gettimeofday()` solely to create the per-query NTP
transmit timestamp. A standalone `gettimeofday` probe reproduced the
pre-main stall. The native path now uses `timer.device` UNIT_MICROHZ and
`TR_GETSYSTIME` through `amintp_time_now()`; the host path retains
`gettimeofday()` behind `time_source_host.c`. Amiga timer seconds are treated
as Amiga epoch and converted explicitly with the existing NTP/Amiga epoch
offset. Native nm scans contain no unresolved `gettimeofday`.

The corrected Q closure (SHA-256
`93b60c0353ebcec426034b6ace9c72061ba96580427c8ab842f577038326d9a7`) passed
3/3 deterministic boots. Full-object diagnostic E1 (SHA-256
`2332ffd9612fe9f25ec55ac7a0a99e3c25dcb4680c55d537bb19c99ebd52d364`) passed
3/3. A fresh native AmiNTP build (SHA-256
`1229b1ed7236d908f5581a7166be56ba19d3d0bd434cec5b6fb15d3c2cbfd8f9`) passed
VERSION twice with `bsdsocket_library=0` and twice with `=1`, returning
`AmiNTP 0.3.2-m3.2` and RC 0 each time. Host/static and hardened UDP
regression passed. Networking qualification is the next required stage.

## First network attempt

After the native startup gate cleared, a numeric IPv4 query was attempted twice
with `QUERY SERVER=85.24.237.71 TIMEOUT=3 RETRIES=1` under
`bsdsocket_library=1`. The first run reached BOOT/LIBS/BEFORE but timed out
without RC/AFTER/DONE; the run was classified TIMEOUT and no SNTP PASS is
claimed. A standalone timer.device probe (`TR_GETSYSTIME`, UNIT_MICROHZ)
passed, so the new time source itself is not the observed network blocker.
DNS and further network tests are suspended pending independent confirmation of
external UDP/123 availability through FS-UAE bsdsocket.
