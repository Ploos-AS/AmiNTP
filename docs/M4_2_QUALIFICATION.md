# M4.2 AmiTCP runtime qualification

Overall: **BLOCKED**, 2026-09-08. Real AmigaOS and the installed AmiTCP_NG
library were exercised. No successful external SNTP exchange was observed.
Neither AROS nor FS-UAE's host socket emulation is counted as AmiTCP evidence.

## Revision, build and environment

Starting HEAD: `b7699d570be39fdf2c777d79628199925211bb77` (M4.1).
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

No product fix is claimed. Inspection found these outstanding issues:

- `net_amiga.c` discards the sender sockaddr in `recvfrom`: source IPv4 and UDP
  source port are not validated. The socket is not connected to the peer.
- `query.c` supplies an all-zero request transmit timestamp. Although `sntp.c`
  compares the response originate field, an unrelated valid-looking response
  with zero originate can pass. **The implementation cannot safely distinguish
  the expected reply from an unrelated UDP packet.**
- A 48-byte receive buffer can truncate a longer datagram to 48 bytes, hiding its
  original length from the otherwise strict parser. Short replies are rejected.
- Parser checks mode, version, leap, stratum and originate; these checks were
  not relaxed. A zero transmit/server timestamp is subsequently rejected by
  epoch conversion. Existing tests exercise malformed protocol fields.
- Numeric CLI/config PORT is constrained to 1..65535. TIMEOUT is 1..3600 and
  RETRIES 0..20. The loop implements retries plus the first attempt, but DNS is
  synchronous and is outside the WaitSelect timeout. Send errors skip waiting.
- Library-open, DNS and socket failures return 10; opened sockets/libraries are
  closed on the inspected exit paths. Actual missing-library failure was not
  exercised. CLI preserves query RC; ARexx error state was observed above.

Peer address/port validation, a per-query nonzero correlation timestamp, and
oversize-datagram detection belong in the unfinished M4.2 hardening work, with
regression cases for wrong peer/port/originate and short/oversized datagrams.
They are explicitly **deferred**, not fixed or qualified by this commit:
external AmiTCP networking is unavailable, so the required affected real-runtime
rerun cannot currently be completed. Resolve these before closing M4.2 or using
its successful SYNC path as qualification evidence. No security guarantee is
implied by unauthenticated SNTP even after those checks.

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
  --workbench '/path/to/existing/Workbench' \
  --net '/path/to/existing/Net' --rom '/path/to/existing/a1200.rom' \
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
