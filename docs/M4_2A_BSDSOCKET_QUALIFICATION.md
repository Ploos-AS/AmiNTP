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

## bsdsocket transport boundary

Linux host UDP verification succeeded for `85.24.237.71:123`: 48-byte NTP
reply, source `85.24.237.71:123`, version 3/mode 4, approximately 93 ms.

A native guest socket probe under `bsdsocket_library=1` passed
`OpenLibrary("bsdsocket.library",4)` three times in setup, but `socket(AF_INET,
SOCK_DGRAM,0)` never returned. Two captured runs reached BOOT/LIBS/BEFORE,
created E1_MAIN and OPEN_OK, then timed out without SOCKET_OK/RC/AFTER/DONE.
This places the current M4.2a blocker at the FS-UAE bsdsocket `socket()` call,
before DNS, sendto, WaitSelect, recvfrom, or AmiNTP protocol handling. No
application networking change is justified.

## Current FS-UAE requalification

The deterministic runner now supports the repaired M3.4-style bootstrap and
uses unique disposable run directories. With FS-UAE 3.2.35, A1200/020,
2 MiB Chip, 8 MiB Fast, Kickstart 40.68, Workbench 40.42, and the A1200
Workbench volume, the trivial process gate passed with both
`bsdsocket_library=0` and `=1`. The native VERSION gate also passed in both
modes, returning `AmiNTP 0.3.2-m3.2` and RC 0.

The full-system assign experiment showed that assigning `SYS:` itself stalls
this minimal disposable startup; the working qualification path therefore uses
the proven absolute `DH1:C/Assign LIBS: DH1:Libs` bootstrap. This does not
change the Workbench files or AmiNTP.

A fresh independent socket probe again opened `bsdsocket.library` and reached
`E1_MAIN`/`OPEN_OK`, but `socket(AF_INET, SOCK_DGRAM, 0)` did not return before
the 30-second watchdog. The run was classified TIMEOUT. Consequently the
M4.2a network matrix remains BLOCKED at the independent FS-UAE socket boundary;
DNS, SNTP, configuration, clock sync, and ARexx network tests remain
UNVERIFIED. Amiberry results remain supplemental and are not used as the
FS-UAE release gate.

## Socket ABI isolation

A fresh standalone probe (`socket_diag`, SHA-256
`00f07bce466dea25193ab3691bbcdc14802c9c213aba7d91747bbac6556f88fe`) was
compiled with Bebbo GCC `-m68000 -mcrt=nix20`. It uses the canonical NDK
`proto/bsdsocket.h` inline interface with
`BSDSOCKET_BASE_NAME AmiNTPSocketBase`, and defines that library base exactly
as the inline stubs require. Disassembly shows the expected Amiga library call:
the domain/type/protocol are placed in D0/D1/D2 and the call is `jsr a6@(-30)`.

The probe creates and closes marker files around every operation. With
`bsdsocket_library=1`, it consistently reaches `OPEN_OK` and
`BEFORE_SOCKET`, then hangs inside `socket(AF_INET, SOCK_DGRAM, 0)`; no
`SOCKET_OK`, close, or completion marker appears before the watchdog. A TCP
variant (SHA-256
`68ceb94e15204546f0147c8ec53f2564ec6a9f3ccb08b2dc9386f32858f85544`) hangs at
the same boundary. The same UDP probe with `bsdsocket_library=0` also opens the
guest `bsdsocket.library` and hangs at the first socket call, so the guest
library path itself is not a normal open failure in this Workbench tree.

This is independent of AmiNTP and does not indicate an application ABI defect.
The result is currently classified as **still unresolved**, with the narrowest
observed boundary being the FS-UAE-provided bsdsocket socket implementation
after a correct canonical library-vector call. No runtime initialization call
has been added; the installed NDK pattern provided no demonstrated requirement
for an extra pre-socket initialization step.

## Host/runtime isolation follow-up

The exact UDP probe was reproduced once more with the generated configuration
(`bsdsocket_library = 1`). It reached `OPEN_OK` and `BEFORE_SOCKET`, then timed
out. A TCP build behaved identically. A control probe using the same canonical
inline interface reached `OPEN_OK` and `BEFORE_INET`, then hung in
`inet_addr("127.0.0.1")`, showing that the problem is not specific to UDP
socket creation alone; host/network-touching bsdsocket vectors are affected.

The host control environment can create IPv4 UDP/TCP sockets when run with the
required host permissions. During a guest hang, non-invasive elevated `strace`
attached to the owned FS-UAE process (28 threads): the emulator was spending
the sample in futex waits while the main process remained active. No host
`socket()`/connect attempt was observed in the selected trace set. This is
consistent with an emulator-side synchronization/backend wait, but does not
identify an internal FS-UAE function.

The standalone probe was rebuilt with `-mcrt=nix13` (SHA-256
`605c434e14a33eb79809620366d722615d852ef3f043d8e4c590108f0cb6fae7`) and
`-mcrt=clib2` (SHA-256 not retained as a qualification artifact). Both reached
the same pre-socket markers and timed out. Thus the result is not specific to
the `nix20` startup variant. The installed NDK inline disassembly remains the
canonical `jsr a6@(-30)` call with D0/D1/D2 arguments and
`AmiNTPSocketBase`.

An A4000/020 control boot using the A4000 ROM and the A1200 Workbench did not
reach boot markers, so no machine-profile socket conclusion is drawn from that
attempt. No alternate licensed Workbench environment was available for a
controlled comparison. The narrow classification remains **unresolved
emulator/runtime blocker**, with FS-UAE bsdsocket/backend synchronization the
leading evidence-supported location; AmiNTP networking code is unchanged.

## Independent bsdsocktest cross-check

The upstream open-source `tbdye/bsdsocktest` repository was checked out
externally at commit `cb08680843bc9cff93d57ca4760e59ab71e93b57` (MIT license;
source retained outside this repository). Its documented build uses
`-noixemul -O2 -Wall -Wextra -m68020 -fomit-frame-pointer`.

The unmodified upstream build does not compile with the installed Bebbo NDK:
the NDK inline `SocketBaseTags` varargs macro expands `_sfdc_vararg`, which this
compiler/header combination does not define. Defining `NO_INLINE_VARARGS` gets
through compilation but leaves `SocketBaseTags` unresolved at link time. No
bsdsocktest binary was therefore produced, and LIST/utility/socket suite
results are **UNVERIFIED**. This is a toolchain/header integration limitation,
not evidence that bsdsocktest passed or failed at runtime.

The requested emulator A/B could not be performed because no practical second
FS-UAE build was available locally. Accordingly, no 3.2.35 regression range or
fix commit is claimed. The independent canonical probes remain the observed
runtime evidence: UDP and TCP socket calls hang, while OpenLibrary succeeds.

## Explicit SocketBaseTagList initialization probe

The installed NDK defines `SocketBaseTagList(struct TagItem *)` in
`clib/bsdsocket_protos.h` and the inline implementation in
`inline/bsdsocket.h` at LVO `-294`. `SBTC_ERRNOPTR(sizeof(LONG))` expands to
the documented long-error-pointer code through `SBTM_SETVAL`; the TagItem list
is terminated by `TAG_DONE`. The same headers define the varargs
`SocketBaseTags()` wrapper, but that wrapper is unusable with this toolchain's
missing `_sfdc_vararg` support.

A standalone non-varargs probe was built with `-m68000 -mcrt=nix20`, SHA-256
`cec7f29989af96b1badafa174bcac284162217066384ebd6cc3c28f93219b1d1`. Its
disassembly contains `jsr a6@(-294)` for `SocketBaseTagList` and `jsr a6@(-30)`
for `socket`, with no `_sfdc_vararg` dependency. Under FS-UAE 3.2.35 it reaches
`OPEN_OK` and `BEFORE_TAGLIST`, then hangs inside `SocketBaseTagList`; no
`AFTER_TAGLIST` marker appears. Therefore the TagList call itself is the first
real bsdsocket dispatch that blocks. The initialization hypothesis is not
confirmed as a remedy; the failure occurs before `inet_addr()` or `socket()`.

This strengthens the classification to an unresolved FS-UAE/guest bsdsocket
runtime dispatch blocker. AmiNTP networking code was not changed, and the
numeric QUERY gate remains unstarted.

## FS-UAE 3.2.35 source trace

The exact installed baseline source is the official FS-UAE repository
`https://github.com/FrodeSolheim/fs-uae`, tag `v3.2.35`, commit
`4ae7ddaec50b567ed80d71ffbff067cb58e945a3`. The relevant implementation is
in `src/bsdsocket.cpp` and `src/od-fs/bsdsocket_posix.cpp`.

The guest vectors map as follows:

* `SocketBaseTagList` LVO `-294` -> `bsdsocklib_SocketBaseTagList()`;
* `inet_addr` -> `bsdsocklib_inet_addr()` -> `host_inet_addr()`;
* `socket` LVO `-30` -> `bsdsocklib_socket()` -> `host_socket()`.

`bsdsocklib_SocketBaseTagList()` first obtains the per-task base with
`get_socketbase(context)`, then repeatedly reads guest TagItems with
`get_long()`. `bsdsocklib_socket()` obtains the same per-task base and then
calls the POSIX `host_socket()`, whose first host operation is the native
`socket(af,type,protocol)` call. `host_inet_addr()` similarly converts the
guest pointer and calls the native `inet_addr()` directly. This identifies the
earliest shared path as the native trap/LVO dispatch and per-task-base access;
the three calls diverge before their individual host semantics.

The source's per-task state is allocated during `bsdsocklib_Open()` by
`alloc_socketbase()`. That allocates a signal, descriptor tables, and invokes
`host_sbinit()`, which creates a pipe, initializes a semaphore, and starts the
`bsdsocket` worker thread. The observed host `strace` sample showed the
FS-UAE process and its threads in futex waits, while no native host socket call
appeared. No source-level evidence currently proves that the worker is the
owner of the first `SocketBaseTagList`/`inet_addr` wait; an instrumented custom
FS-UAE build was not produced in this run.

The source configuration exposes bsdsocket through the compile-time
`BSDSOCKET` feature (`configure.ac` `OPT_FEATURE([BSDSOCKET], ...)`). The
installed binary's `ldd` output did not expose a separate libslirp dependency,
and no hidden guest configuration requirement was identified from the source.
The exact first missing transition therefore remains unresolved at the
FS-UAE trap/dispatch boundary; no AmiNTP change is justified.

## Exact FS-UAE 3.2.35 source trace

The external source tree is checked out at official tag `v3.2.35`, commit
`4ae7ddaec50b567ed80d71ffbff067cb58e945a3`. Bootstrap and configure completed
with the default Linux feature set and `BSDSOCKET=1`. A self-built binary was
not completed within this run (the build was still compiling when the bounded
build window ended), so installed-versus-self-built runtime equivalence is
UNVERIFIED.

Source inspection gives the concrete dispatch path:

```
LVO -294 -> bsdsocklib_SocketBaseTagList()
             -> get_socketbase(context)
             -> get_long() over guest TagItems

LVO -30  -> bsdsocklib_socket()
             -> get_socketbase(context)
             -> host_socket()
             -> native socket()

inet_addr -> bsdsocklib_inet_addr()
             -> host_inet_addr()
             -> native inet_addr()
```

`alloc_socketbase()` is called from `bsdsocklib_Open()`. It allocates the
per-task state, obtains an Exec signal, initializes descriptor tables, and
calls POSIX `host_sbinit()`. `host_sbinit()` creates the abort pipe,
initializes the semaphore, and starts the `bsdsocket` worker thread. Thus the
worker setup is expected to occur during OpenLibrary, before the first vector.

The guest trace reaches `OPEN_OK` and `BEFORE_TAGLIST`, but no return marker.
No temporary FS-UAE instrumentation build was produced, so the exact internal
statement, worker request, or trap-return transition remains unobserved. The
earliest source-level common boundary supported by both code and runtime
evidence is entry into `bsdsocklib_SocketBaseTagList()` and its initial
`get_socketbase()`/guest-memory path. No new guest probe was added.

## Self-built 3.2.35 and dispatch instrumentation

The exact external v3.2.35 tree was built after the earlier bounded window ended during the final link step. The resulting source-built emulator was run with the same guest, ROM, configuration, and explicit TagList probe; it reproduced the installed behavior: `OPEN_OK` followed by a timeout at `BEFORE_TAGLIST`.

A temporary external instrumentation patch added host `write_log()` markers to `bsdsocklib_SocketBaseTagList()` around `get_socketbase()`, each first `get_long()` TagItem read, and the return path. The instrumented binary was run with `log_bsdsocket=1`. FS-UAE logged emulated library creation and `OpenLibrary()`, but emitted none of the TagList markers before the guest watchdog expired. Thus the handler body was not observed to execute; the first missing transition is between the guest LVO call and entry into `bsdsocklib_SocketBaseTagList()`, rather than a demonstrated TagItem parsing or handler-return statement.

The source-built baseline confirms the installed 3.2.35 result but does not yet identify the internal trap dispatch/guest resume defect. External source modifications remain outside this repository. M4.2a remains blocked and AmiNTP source is unchanged.

## LVO/trap dispatch boundary

In v3.2.35, `bsdlib_install()` builds `sockfuncvecs[]` from the ordered
`sockfuncs[]` table. Each entry stores `here()`, emits `calltrap(deftrap2(...))`,
and emits `RTS`. The function table then places Open/Close/Expunge at indices
1..3 and the remaining entries at their corresponding negative LVOs. Thus
`socket` is index 4/LVO `-30`, `inet_addr` is index 31, and
`SocketBaseTagList` is index 48/LVO `-294`; both map to the named handlers in
that table. `calltrap()` emits the UAE `0xA000 | trap_id` opcode, and the CPU
recognizes that opcode in `newcpu.cpp` before calling `m68k_handle_trap()`.

The instrumented external build added immediate logging to both
`bsdsocklib_SocketBaseTagList()` and `m68k_handle_trap()`. With the explicit
TagList probe, FS-UAE logged emulated library creation and OpenLibrary, but
emitted neither `TRAP ENTER` nor `TAGLIST ENTER` before the watchdog. The
runtime therefore does not reach generic trap dispatch, much less handler
lookup or TagItem parsing. Runtime guest vector bytes, stub address, trap ID,
and PC/A6 at the hang were not dumped; they remain UNVERIFIED. Source-level
mapping is complete, but the first missing transition is the guest execution of
the generated `0xA000|trap_id` stub between `jsr -294(a6)` and
`m68k_handle_trap()`.

The source also shows extended trap handling creates a trap context and uses
`switch_to_trap_sem`/`switch_to_emu_sem`, but that path is downstream of the
unobserved generic trap entry. No evidence justifies attributing the current
hang to worker or TagItem logic. M4.2a remains blocked at the UAE trap-opcode
recognition/dispatch boundary.

## Runtime vector inspection

A targeted guest dump after `OpenLibrary("bsdsocket.library",4)` recorded the
returned base and negative vectors. The observed bytes were:

```
SocketBase-30:  4E F9 00 F0 21 40
SocketBase-294: 4E F9 00 F0 21 F0
```

Both are absolute JMP vectors. The `-30` target (`00F02140`) contained
`A0 3D 4E 75` (UAE calltrap opcode followed by RTS). The `-294` target
(`00F021F0`) read as eight zero bytes, despite the source table registering
`SocketBaseTagList` at index 48/LVO `-294` with a generated calltrap stub.
Thus the runtime TagList vector points at a zero-filled/non-stub target rather
than a valid A-line trap. This is direct guest-memory evidence of generated
vector/stub corruption or incomplete installation. Runtime SocketBase was not
persisted as a stable address artifact, and CPU PC/A6 capture was not required
to establish the invalid target.

The source-built and installed emulators both exhibited the pre-existing
`BEFORE_TAGLIST` hang; no AmiNTP code was changed. The narrow classification is
**BSDsocket generated calltrap stub/vector corruption at LVO -294**. Further
qualification is blocked pending an FS-UAE fix or a supported emulator build.

## Corrected complete vector dump

A subsequent corrected dump avoided a probe-side pointer-arithmetic error in the earlier target inspection. The runtime SocketBase was `0x002192D4`, with `lib_NegSize = 0x12C` (300 bytes), `lib_PosSize = 0xB0`, version 4, revision 1. The `-294` vector bytes are `4E F9 00 F0 21 F0`, targeting `0x00F021F0`; that target contains `A0 68 4E 75` (trap ID `0x68`, RTS), not zeros. The earlier zero-filled-target conclusion is superseded and was caused by the diagnostic dump's incorrect pointer calculation.

The source `sockfuncs[]` table contains 50 entries (indices 0..49), ending at `bsdsocklib_GetSocketEvents`; the negative library size is exactly 50 * 6. All sampled vector slots through the final table entry contain absolute JMP vectors to sequential generated stubs. Stub emission and vector-table sizing are therefore consistent at runtime; no cutoff or reservation overflow has been demonstrated. The calltrap/CPU dispatch boundary remains unresolved.

## Self-built CPU A-line boundary follow-up

The self-built v3.2.35 binary was rerun with JIT disabled (`jit_compiler=0`)
and temporary external instrumentation in both `op_illg_1`/`op_illg` and
`m68k_handle_trap()`. The explicit probe again reached `BEFORE_TAGLIST` and
was terminated by the watchdog. No A-line trace, generic trap-entry trace, or
TagList-handler trace was emitted. This is consistent with the valid runtime
stub bytes recorded above, but does not prove whether the emulated CPU fails to
fetch the rtarea target, dispatches through another CPU path, or blocks before
`op_illg_1`; the exact guest PC and trap-return transition remain unobserved.
The current narrow classification is therefore **FS-UAE bsdsocket calltrap
boundary unresolved**, with no AmiNTP source change and no demonstrated
application defect. Further work requires CPU execution/PC instrumentation in
the external emulator rather than additional guest API probes.

## CPU execution tracing follow-up

The explicit TagList probe was rerun against the self-built v3.2.35 emulator
with `jit_compiler=0`. Temporary tracing was added to the A-line illegal-opcode
path, generic trap entry, and the normal interpreter fetch/execute loop for
`0x00F02100..0x00F02220`. The guest again reached `BEFORE_TAGLIST` and timed
out; no CPU-range fetch, A-line, generic-trap, or handler marker was emitted.
The runtime memory dump still shows `SocketBase-294 -> 0x00F021F0` and
`A0 68 4E 75`. Therefore the first missing transition is narrowed only to CPU
execution/fetch of the rtarea target (or an alternate execution engine path not
covered by these hooks). Guest PC/A6 at watchdog and symbolic CPU-thread stack
remain unverified. No AmiNTP source was changed; M4.2a remains blocked.

## Guest call-site disassembly

The explicit TagList probe disassembles the relevant sequence as:

```
0x2F4  move.l  0x28,d0
0x2FA  movea.l d0,a6
0x2FC  movea.l a5@(-16),a0
0x300  jsr     a6@(-294)
0x304  move.l  d0,a5@(-20)
```

Thus the static call-site offset is `TAGLIST_CALL_PC=0x300` and the expected
post-call return offset is `TAGLIST_RETURN_PC=0x304` within the probe text
segment. The preceding code loads A6 from the SocketBase global and passes the
TagItem pointer in A0. A runtime register capture and relocated absolute guest
PC were not obtained: the self-built emulator still times out after
`BEFORE_TAGLIST` without producing CPU fetch, A-line, or trap markers. The
runtime vector/stub evidence remains valid (`SocketBase-294 -> 0x00F021F0`,
`A0 68 4E 75`).

## Fresh four-way calltrap control

A fresh standalone Bebbo `-m68000 -mcrt=nix20` diagnostic (`/tmp/matrix`,
external to the repository) dynamically resolved both library targets after
OpenLibrary. With empty TagItem and errno-pointer TagItem controls, the observed
results were:

| Control | Path | Result |
|---|---|---|
| TagList normal | normal `SocketBaseTagList()` LVO | RETURN |
| TagList direct | direct call to resolved `0x00F021F0` | RETURN |
| socket normal | normal `socket(AF_INET, SOCK_DGRAM, 0)` | RETURN |
| socket direct | direct call to resolved socket stub | HANG |

The direct socket call does not establish the library-base register expected by
the bsdsocket ABI, so its hang is not treated as a valid calltrap comparison.
This fresh control does demonstrate that valid TagList calltrap execution and a
normal socket LVO call can return in the same FS-UAE profile. It conflicts with
the older `/tmp/tagprobe` timeout and therefore leaves the original failure
reproducibility unresolved; no AmiNTP source was changed and M4.2a remains
blocked pending reconciliation of the probe difference.

## Probe reconciliation run

The historical `/tmp/tagprobe` (`cec7f299...`) was rerun under the canonical
FS-UAE 3.2.35 profile and timed out after `BEFORE_TAGLIST`; it did not reach
its later socket call. The fresh `/tmp/matrix` probe (`79fb3d7f...` at the
initial build) was run under the identical profile and returned from normal
TagList and normal UDP `socket()` calls. Repeating the order old/fresh/old/fresh
produced TIMEOUT/PASS/TIMEOUT/PASS, so the discrepancy is binary/probe-specific
rather than a demonstrated boot-order effect.

The fresh canonical normal socket control was repeated five times (runs r1-r4
and r6): all five returned normally and completed cleanup. Its direct socket
stub control still hangs and is ABI-inconclusive because it bypasses the normal
library-base setup. The old probe's exact cause is not yet isolated; its
observed hang boundary is TagList, not socket. No AmiNTP source changed.

## Historical diagnostic false lead resolved

The old `/tmp/tagprobe.c` opened the library into a local variable:

```c
b = OpenLibrary("bsdsocket.library", 4);
```

but never assigned the NDK base symbol used by the inline calls:

```c
struct Library *AmiNTPSocketBase;
```

Consequently `SocketBaseTagList(tags)` used a null/stale `AmiNTPSocketBase`,
while the fresh `/tmp/matrix.c` explicitly performs `AmiNTPSocketBase = b`.
The old probe therefore hung at TagList; it never reached its later socket call.

A one-line old-probe repair adding `AmiNTPSocketBase=b` produced binary
`/tmp/tagprobe_fixed` (SHA-256
`97cb4808b199ca21209477462038af1628fe00184c216ad959a47f2f3968d473`) and
returned through TagList, `inet_addr`, and the normal socket LVO on five fresh
runs. The canonical fresh normal socket control was also PASS 5/5, and the
fresh `inet_addr("127.0.0.1")` control was PASS 5/5. This proves the historical
hang was an invalid SocketBase ABI setup in the diagnostic probe, not an FS-UAE
bsdsocket defect. No AmiNTP source changed.

## Real AmiNTP runtime resumed

A clean native rebuild produced AmiNTP SHA-256
`1229b1ed7236d908f5581a7166be56ba19d3d0bd434cec5b6fb15d3c2cbfd8f9` with
Bebbo GCC `-m68000 -mcrt=nix20`; the binary is AmigaOS loadseg format and has
no unresolved ixemul/gettimeofday symbol.

Under the canonical FS-UAE 3.2.35 profile, `AmiNTP VERSION` passed with
`AmiNTP 0.3.2-m3.2`, RC 0, and clean markers. The repaired standalone control
returned through TagList, `inet_addr`, and the normal socket call; in this
profile its socket result was a normal failure (`SOCKET_FAIL`), not a hang.

The first real numeric query (`QUERY SERVER=85.24.237.71 TIMEOUT=3 RETRIES=1`)
timed out after the runner's `BEFORE` marker, with no application output. The
current Amiga implementation calls `gethostbyname()` before `socket()` even for
a numeric string, so the first application boundary is unresolved within the
name-resolution path. No production source was changed and no claim of SNTP
qualification is made. M4.2a remains blocked pending a controlled resolver/
network fixture or evidence-driven application diagnosis.

## Numeric IPv4 resolver fast path

`src/net_amiga.c` and the host backend now parse strict dotted-quad IPv4
literals before resolver use. Valid literals are converted directly into
`struct in_addr`; hostnames retain the existing `gethostbyname()` path and
validation. The parser accepts exactly four decimal octets in `0..255`, with
no trailing characters, and has deterministic host tests for valid literals,
malformed literals, and hostname strings.

The native rebuild (Bebbo GCC, `-m68000 -mcrt=nix20`) produced AmiNTP
SHA-256 `b265d31c8ab7f07a64d20f5b05eb24ae9f033dddc1195fdd3e2b51fd9d35a012`.
`make check` and `make m4.2-check` pass. VERSION passes under FS-UAE 3.2.35.
A standalone parser probe returns, but the first real numeric AmiNTP QUERY still
watchdogs before application output; no successful SNTP exchange has yet been
observed. The next boundary is therefore in the real AmiNTP query path after
CLI/config handling, and no further network gates were claimed.

## Real-query boundary trace

Temporary markers were added around the production Amiga UDP path and the same
numeric QUERY was rerun with a 35-second watchdog (`TIMEOUT=5`, `RETRIES=2`,
so the expected network wait budget is approximately 15 seconds plus margin).
The run emitted no `Q02..Q10` markers and produced no application output before
watchdog termination. This means the call did not reach `amintp_udp_query()`;
the current boundary is earlier in CLI/configuration or command startup, not
proven to be parser, socket, send, or WaitSelect behavior. Temporary tracing was
removed and no production diagnostic markers were committed.

## Config-stage and production-query markers

Temporary top-level markers proved the numeric CLI QUERY reaches and returns
from `amintp_load_config("ENVARC:AmiNTP/AmiNTP.conf", ...)`; the file-open
attempt returned failure normally (`C10_FOPEN_FAILED`), followed by
`M05_CONFIG_RETURN` and `M07_QUERY_DISPATCH_BEGIN`. Thus ENVARC fopen is not the
pre-query hang.

A second temporary build with markers in `amintp_udp_query()` reached:
`Q00_ENTER`, `Q01_TIME_DONE`, `Q02_OPEN_OK`, `Q03_PARSE_BEGIN`,
`Q04_PARSE_DONE`, and `Q05_SOCKET_RETURN`, then watchdoged. This establishes
that the numeric parser and socket call return; the remaining boundary is the
error/cleanup path after the socket result (the profile returned a failed socket
rather than a usable descriptor). Temporary markers were removed. No further
network qualification is claimed because no deterministic UDP fixture was
available and the query did not reach send/receive.

## Socket failure and cleanup boundary

A focused production trace with `SERVER=85.24.237.71 PORT=123 TIMEOUT=1
RETRIES=0` observed:

```
S00_BEFORE_SOCKET
S01_AFTER_SOCKET
S02_SOCKET_FAILED
S05_BEFORE_CLOSELIBRARY
S06_AFTER_CLOSELIBRARY
```

The signed socket result is `-1` in the equivalent standalone lifecycle probe;
errno remains zero without explicit errno TagList setup and becomes `1` with the
known errno-pointer TagList. The production path therefore receives a normal
failed socket result, enters cleanup, and returns from `CloseLibrary()`; the
cleanup call itself is not the hang boundary. The remaining stall is after
`CloseLibrary()` and before normal query-command completion. No SNTP traffic was
attempted and no production source change was made in this diagnostic pass.

## Host socket permission boundary

The installed Amiga SDK maps errno value `1` to `EPERM` (`sys-include/sys/errno.h`
and clib2 `errno.h`: `#define EPERM 1`). In the ordinary execution context,
host Python IPv4 UDP and TCP socket creation both fail with errno 1,
`Operation not permitted`. With host socket permission enabled, both succeed.

The same A/B applies to FS-UAE: under the ordinary context the guest socket
returns `-1`; under the permitted context the initialized socket lifecycle
probe returns `FD=0`, closes the descriptor, and `CloseLibrary()` returns.
This proves the previous guest failure was inherited host sandbox policy, not an
FS-UAE bsdsocket defect. The real AmiNTP public numeric query was retried in the
permitted context but no deterministic SNTP fixture was available, so no SNTP
success is claimed.

## Permitted-context socket preflight and fixture attempt

Host preflight in the permitted execution context succeeded for both IPv4 UDP
and TCP socket creation. The canonical initialized guest UDP lifecycle likewise
returned a valid descriptor (`FD=0`) and completed `CloseSocket`/
`CloseLibrary`.

A deterministic SNTP fixture was started on the host at UDP port 49234. A real
AmiNTP numeric query targeted `10.0.2.2:49234` with `TIMEOUT=2 RETRIES=0`, but
the fixture received no packet and the guest run timed out. The assumed gateway
address is therefore not yet proven as the guest-visible host endpoint for this
FS-UAE bsdsocket configuration. No SNTP or AmiNTP protocol conclusion is drawn;
qualification remains blocked at fixture reachability.
# M4.2a numeric qualification follow-up

With Bebbo `m68k-amigaos-gcc` 6.5.0b (20260807212032), the combined
two-field timestamp comparison can generate a post-incremented request pointer;
the subsequent fraction access then uses effective offset +8 instead of +4.
The minimal workaround keeps seconds and fraction loads in separate locals.
The resulting assembly uses the required request fraction offset +4.

The workaround passed the valid loopback numeric QUERY gate 5/5 (RC0,
`NTP_FRACTION=1073741824`, `AMIGA_MICROS=250000`, AFTER/DONE, no watchdog),
the malformed-origin rejection gate (RC10), and the timeout gate (RC10).
