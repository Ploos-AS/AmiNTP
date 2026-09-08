# M4.2 AmiTCP runtime qualification

Overall: **BLOCKED**, 2026-09-08 (Aminet driver probe). Real AmigaOS and the installed AmiTCP_NG
library were exercised. No successful external SNTP exchange was observed.
Neither AROS nor FS-UAE's host socket emulation is counted as AmiTCP evidence.

### Hardware-profile correction

The qualification runs documented here used the existing A1200/68020 profile.
That is not a physically valid A2065 target: Commodore's A2065 is a Zorro-II
big-box expansion card for systems such as the A2000/A3000/A4000, whereas the
A1200 does not provide a Zorro-II slot. FS-UAE's log can expose an emulated Zorro
card while retaining the A1200 model, but that does not make the profile an
A2065-compatible Amiga configuration. The Lance-Test failure and both driver
failures must therefore be treated as an invalid-hardware-profile result, not
as final evidence that a real A2065 or a correctly emulated big-box system
cannot work. A future M4.2 attempt must use a big-box FS-UAE profile with a
Zorro-II bus (and corresponding AmigaOS system configuration) before drawing
driver or AmiTCP conclusions.

### Valid big-box rerun

This requirement was then met with FS-UAE `A4000`, CPU override `68020`, 2 MiB
chip RAM and 8 MiB fast RAM, Kickstart 40.68 A4000 and the existing Workbench
40.42 installation. FS-UAE again logged `A2065 Z2 Ethernet` with
`network_card=a2065`, `a2065=slirp`, and `bsdsocket_library=0`. This is the
valid-profile evidence below; the earlier A1200 results remain historical only.

On the A4000 profile, Lance-Test still detected the malformed
`00:FFFFFF80:10:32:33:34` address, passed buffer memory, and failed the LANCE
configuration test. The official 2.14 driver still returned `IOERR_OPENFAIL`
from the direct probe. Therefore the A1200 form factor was not the cause of the
observed failure; the current FS-UAE A2065 emulation or its compatibility with
these historical drivers remains the blocker.

## Revision, build and environment

Starting HEAD for the prior report: `b7699d570be39fdf2c777d79628199925211bb77` (M4.1).
This rerun started at `abcd52a830da5b36a44177bbd08003ec5ef0959a`.
Executed `git fetch origin`, `git checkout main`, `git pull --ff-only origin main`
in that order, then recorded HEAD and verified a clean worktree. Git metadata
writes required sandbox escalation. Application sources remain at that revision.
The final report/harness commit is identified by
`git log -1 --format=%H -- docs/M4_2_QUALIFICATION.md`; final HEAD, origin/main,
divergence and clean status are recorded in the completion response.

- Installed FS-UAE 3.2.35; A1200/020, CPU 68020, no FPU, 2 MiB chip / 8 MiB fast.
- Guest: Kickstart 40.68, Workbench 40.42 (AmigaOS 3.1), exec.library 40.10,
  dos.library 40.3. Real RexxMast: ARexx Version 1.15.
- Installed library file: `AmiTCP_NG 4.1.5 (08-Aug-26)`.
  Opened library: `bsdsocket.library 4.1 (AmiTCP_NG 4.1.5)`.
  `GetNetStatus` returned 0, identifying AmiTCP/IP release 3 and this library.
- Original A1200 profile has `bsdsocket_library = 1`. Qualification explicitly
  sets it to **0**, preventing host socket emulation from qualifying as AmiTCP.
- Existing Workbench and Net directory mounts are read-only. The writable boot
  directory contains only our binary, harness, disposable config and outputs.
  No original startup, stack, user config, ROM or system file was modified.
- Bebbo: `/opt/amiga/bin/m68k-amigaos-gcc`, GCC 6.5.0b 20260807212032.

Fresh native build, exit 0:

```sh
make -B all CC=/opt/amiga/bin/m68k-amigaos-gcc \
  CFLAGS='-Os -Wall -Wextra -Werror -m68000 -mcrt=nix20' \
  LDFLAGS=-mcrt=nix20
```

`file AmiNTP`: `AmigaOS loadseg()ble executable/binary`.
Built and guest binary SHA-256:
`cd78e666e8e3517ac87c13af46a31bea0e031cc1c1b70393279364ac7993a50b`.
The build selects libnix at compile and link time; binary string inspection
finds no ixemul dependency. Native execution is independently observed below.

## Network prerequisite: BLOCKED

The installed startup only opens the stack to start loopback; its comments say
hardware NIC setup is manual. `DEVS:NetInterfaces` is empty and there is no
`DEVS:Networks` directory. The stack database contains localhost but no DNS
server configuration. Guest observations:

```text
Network status summary
Local host address         = (Not configured)
Default gateway address    = (Not configured)
Domain name system servers = (Not configured)
```

`netstat` shows only `lo0`, address `127.0.0.1`, state Up, and its host route.
No external interface or default route exists. Consequently the installed stack
is available, but is **not configured for external networking** in this profile.
This is not evidence that a configured AmiTCP stack cannot run AmiNTP.

Required next environment work: provide a supported emulated network device and
compatible guest SANA-II driver, configure its address/default route/DNS, and
verify real outbound UDP/123. Merely reenabling FS-UAE host bsdsocket emulation
would bypass the stack and cannot resolve this qualification blocker. No
proprietary stack downloads or new system installation were attempted.

### A2065 probe

The repeatable harness was extended with `network_card = a2065` while retaining
`bsdsocket_library = 0`. FS-UAE 3.2.35 accepted the option and its log records
`A2065: 'slirp' 00:00:00:32:33:34` and `A2065 Z2 Ethernet`, proving that the
emulator A2065/SLIRP backend initializes. The guest then blocks at
`AddNetInterface Q:A2065`. The supplied template requires `device=a2065.device`,
unit 0, but the read-only Workbench contains no `DEVS:Networks/a2065.device`
and no `uaenet.device`; it contains only an unrelated `DEVS:ethernet.device`.
No SANA-II interface can therefore be opened. The exact missing component is a
compatible, legally redistributable AmigaOS A2065 SANA-II driver installed as
`DEVS:Networks/a2065.device`. None was found locally or copied into the repo.

### Aminet driver attempt

The official [dev/misc/SANA.lha](https://aminet.net/package/dev/misc/SANA) was
downloaded to `/tmp` only. Aminet identifies it as Commodore's official SANA-II
developer package, release 1.4, dated 1992-08-01. Its nested
`sana2developer.lzh` contains `devs/networks/a2065.device`; `ReadMe.BuildNumber`
reports revision V1.4 built 1992-11-12. Archive SHA-256:
`450c4cfc21f38c65e1de46fbb5919a8c4f0a4afc4894fc8c54f15d32c35ec49b`.
Extracted driver SHA-256:
`c6966e30fa7f46b4ab646ed60e6d901e228fd8817dd901ba7833a89c832969cd`.

The preparation harness now requires `--driver`, hashes it into guest evidence,
and copies it only to disposable `Networks/a2065.device`. The original
Workbench remains read-only and neither archive nor driver is tracked. With
`network_card = a2065`, explicit `a2065 = slirp`, and `bsdsocket_library = 0`,
FS-UAE again logged the A2065 SLIRP card. AmiTCP reported loopback-only status,
then `AddNetInterface Q:A2065` produced no RC or post-add status before the
watchdog; the same behavior occurred with and without `requiresinitdelay=yes`.
Driver presence is proven, but device open/interface creation is not; no
address, route, DNS, or UDP proof exists.

### Lance-Test hardware diagnostic

The Aminet `driver/net/LanceTest.lha` package was downloaded to `/tmp` only.
Archive SHA-256 is
`b6e3fd1a49e39b2043197309ae42745a161ae50a140490318b52bf7bdd69fef3`;
the executable is Lance-Test 36.2 (04-Dec-1990), SHA-256
`b45c11007ee491defc4867613a79c643438ddac8118c5bc1c70b9b50f2061d61`.
Run before AmiTCP startup with `Lance-Test diags` produced:

```text
Ethernet address of selected board is:  00:FFFFFF80:10:32:33:34
Buffer memory test.............. PASS
LANCE configuration test........ FAIL
```

The remaining interrupt, collision, and internal-loopback tests did not run
before the watchdog; the MAC output is also malformed. Thus the emulated board
does not pass the vendor hardware diagnostic, independently confirming that
this is an FS-UAE A2065/driver compatibility failure below AmiTCP.

### Later driver comparison

The Aminet `driver/net/a2065v216a.lha` package describes a later updated A2065
driver based on 2.16. Archive SHA-256 is
`f1805bfde98af6014394286500a9672c57e291a8280be5373836d2afd56f0462`;
extracted `$VER: a2065 2.16a (4.3.98)` SHA-256 is
`f0727a46b5a82f95c7a2dbdcde60a66d043ab734e76670c77aa766b498cf7855`.
It was installed only in disposable storage. With this driver, a fresh
probe-only profile produced no guest probe output before the watchdog and the
emulator exited without normal guest completion; no OpenDevice or SANA-II result
can be claimed. This is not success and is recorded separately from the 2.14
`IOERR_OPENFAIL` result.

### Direct SANA-II isolation

The exact disposable interface file passed to AddNetInterface was:

```text
device=a2065.device
unit=0
configure=dhcp
requiresinitdelay=no
```

This matches the installed AmiTCP_NG `Storage/NetInterfaces/A2065` example and
its ReadMe. The harness assigned `DEVS:Networks` to the writable guest path
containing `a2065.device`; the probe's `EXISTS`/copy path is therefore the exact
path used by the device open.

A native Bebbo SANA-II probe, compiled against the official `devices/sana2.h`,
was run separately under a watchdog for `open`, `devicequery`, `station`,
`config`, and `online`. Each run printed `BEFORE_OPEN` and then
`OPENERR=4294967295` (`IOERR_OPENFAIL`, or `-1`). None reached an S2 command;
S2_DEVICEQUERY, S2_GETSTATIONADDRESS, S2_CONFIGINTERFACE, and S2_ONLINE are
therefore **UNREACHED**, rather than successful or hanging. This establishes
the first failing operation as `OpenDevice`, below AmiTCP and before any SANA-II
wire/configuration operation. AddNetInterface itself still hangs after this
failure, until the host watchdog.

## Runtime observations

Exact commands, guest times and RC files are in [evidence](evidence/m4.2/).
All commands use `Q:AmiNTP`; table commands omit that prefix for readability.
Empty redirected stdout is recorded as empty, not as successful output.
AmigaDOS output redirection did not capture libnix stderr; **CLI diagnostic
wording remains UNVERIFIED**. No inferred error text is presented as observed.

| Test | Command | RC / observed stdout | Result |
| --- | --- | --- | --- |
| A | `VERSION` | 0 / `AmiNTP 0.3.2-m3.2` | PASS |
| B | `QUERY SERVER=pool.ntp.org TIMEOUT=5 RETRIES=2` | 10 / empty | BLOCKED: no DNS configuration |
| C | `QUERY SERVER=85.24.237.71 TIMEOUT=5 RETRIES=2` | 10 / empty | BLOCKED: no external route |
| D | `QUERY SERVER=192.0.2.1 TIMEOUT=5 RETRIES=2` | 10 / empty | Nonzero return observed; outside-network timeout behavior UNVERIFIED |
| E | `QUERY` | 10 / empty | Successful config-driven query BLOCKED |
| E override | `QUERY SERVER=85.24.237.71` | 10 / empty | Runtime precedence success UNVERIFIED |
| F | `SYNC ... NORTC` | Not executed | BLOCKED by failed QUERY prerequisites |

The IPv4 address was selected from an actual host `getent ahostsv4 pool.ntp.org`
lookup during qualification. Its UDP/123 reachability was **not established**.
The literal is numeric, but source inspection shows even numeric hosts go
through `gethostbyname`; a dedicated literal conversion path is absent.

The unreachable external address returns within the same guest second, so this
must not be called proof of three five-second waits. A separate real loopback
UDP test uses `QUERY SERVER=127.0.0.1 PORT=49123 TIMEOUT=5 RETRIES=2`, with no
server bound or substituted. It returned RC 10 from 21:34:33 to 21:34:49 (16 seconds), consistent with
three five-second waits plus command overhead. This establishes bounded local
waiting, not packet-level retry counts or external connectivity.
No timeout/retry limits were weakened.

For E, a disposable `ENVARC:` assign points at the qualification disk's `env`
directory, containing `AmiNTP/AmiNTP.conf` with `SERVER=pool.ntp.org`, `PORT=123`,
`TIMEOUT=5`, `RETRIES=2`. Thus the exact M4.1 pathname is used without replacing
any user configuration. Both config-only and CLI-override calls reached failure
RC 10, but these failures alone do not establish all loaded values or precedence.
Host tests establish parser behavior separately.

System time before network probes was `Tuesday 08-Sep-26 21:34:31`. No system-clock or RTC write
was attempted. Before/after SYNC times and successful SYNC output are therefore
**UNVERIFIED**, not omitted evidence of a PASS.

## Real ARexx observations

`Q:AmiNTP RESIDENT` was started in the background; real RexxMast addressed
`AMINTP` with `OPTIONS RESULTS`. This is a new network attempt, not reuse of M3.4.

| Command | RC | RESULT |
| --- | --- | --- |
| `QUERY SERVER=pool.ntp.org` | 10 | unset |
| `SYNC SERVER=... NORTC` | not run | BLOCKED |
| `LASTSYNC` | 0 | `NONE` |
| `LASTERROR` | 0 | `ERROR CODE=QUERY_FAILED` |
| `QUIT` | 0 | `OK QUIT` |

The script drops RESULT before every request, so a stale success cannot be
reported for the failed QUERY. REXX/AMINTP observations are respectively 1/0
before launch, 1/1 while resident, and 1/0 after QUIT. Resident exit RC is 0;
harness RC is 0 (script completion only, **not a network qualification PASS**).
Successful-network LASTSYNC and post-SYNC LASTERROR behavior remain UNVERIFIED.

## Protocol and lifecycle review

The qualification worktree includes focused protocol hardening:

- `net_amiga.c` and `net_posix.c` validate the response IPv4 sender and UDP
  source port against the queried peer.
- `query.c` creates a nonzero monotonic per-query transmit timestamp and uses a
  49-byte receive buffer, so oversized datagrams are rejected by the strict
  48-byte parser.
- Parser checks mode, version, leap, stratum and originate; these checks were
  not relaxed. A zero transmit/server timestamp is subsequently rejected by
  epoch conversion. Existing tests exercise malformed protocol fields.
- Numeric CLI/config PORT is constrained to 1..65535. TIMEOUT is 1..3600 and
  RETRIES 0..20. The loop implements retries plus the first attempt, but DNS is
  synchronous and is outside the WaitSelect timeout. Send errors skip waiting.
- Library-open, DNS and socket failures return 10; opened sockets/libraries are
  closed on the inspected exit paths. Actual missing-library failure was not
  exercised. CLI preserves query RC; ARexx error state was observed above.

Host adversarial coverage passes for valid, wrong-address, wrong-port,
wrong-originate, short, and oversized replies; request timestamps are checked
for nonzero and uniqueness. Real AmiTCP rerun of these checks remains blocked
until the guest SANA-II driver is installed. No security guarantee is implied by
unauthenticated SNTP even after these checks.

## Regression and repeatability

Complete existing host/static regression, exit 0:

```sh
make check host-check m4.1-check m3.3a-check m3.3b-check m3.3c-check
```

M4.1 transitively includes M1, M1.3, M2.1, M2.2, M2.3, M3.1 and M3.2.
Native build and binary execution pass. Existing AROS-compatible harness static
checks pass; AROS runtime was not rerun (no cached media or xvfb-run installed),
and would not supply the missing AmiTCP evidence.

To repeat the probe after the native build, use a fresh output directory:

```sh
python3 ci/local-amigaos/prepare-m4.2.py \
  --machine A4000 --cpu 68020 \
  --workbench '/path/to/existing/Workbench' \
  --net '/path/to/existing/Net' --rom '/path/to/existing/a1200.rom' \
  --driver '/path/to/extracted/a2065.device' \
  --ipv4 CURRENT_NTP_IPV4 --out build/m4.2/new-run
timeout 55s fs-uae build/m4.2/new-run/qualification.fs-uae
```

The script assumes the installed AmiTCP_NG command layout and `Net:AmiTCP`.
It does not install/configure a NIC or execute the original user startup.
Adapt runtime startup only when a real NIC/driver is available. Increase the
host watchdog for a configured network: DNS waits can exceed it. Emulator
exit 124 is the host watchdog, not an AmiNTP RC; require guest `done.txt`,
all outputs, and lifecycle evidence. No automatic overall PASS is emitted.
CLI SYNC NORTC is gated on successful DNS/literal/config/override queries;
ARexx SYNC additionally requires its own successful query. Resolve the review
issues above before running this against a newly connected environment.

The evidence directory contains only our scripts/configuration and non-sensitive
runtime/build output. Original ROM, Workbench, AmiTCP distributions, binaries,
and emulator disks are not committed. The existing installation remains intact.
M4.2 mandatory criteria 2-10 are not all satisfied; overall **BLOCKED**.
