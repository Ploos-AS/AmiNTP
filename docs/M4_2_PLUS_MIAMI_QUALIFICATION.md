# M4.2+ — Miami native-stack qualification

## Result

**PASS**, 2026-09-14.

AmiNTP passes its native-stack integration matrix with Miami 3.2b2 on an
A2000-compatible FS-UAE profile. The guest used a 68000, Kickstart 37.175
(2.04), Workbench 38.36 (2.1), the official `a2065.device` 2.14, and FS-UAE
A2065/SLIRP. FS-UAE revision was
`4ae7ddaec50b567ed80d71ffbff067cb58e945a3`.

This is separate from the M4.2a FS-UAE socket path and the M4.2b AmiTCP_NG
native path. The Miami run used `bsdsocket_library = 0`, a fresh disposable
runtime with no AmiTCP installation or library, and Miami's own
`bsdsocket.library 4.1 (12/15/96)`.

## Miami and platform identity

The installed program reports `Miami 3.2b (11/09/98)` and came from the
official Miami 3.2b2 Aminet distribution: the main archive, the 68000/010
binary archive, and the GTLayout GUI archive. Its local guide and installer
both require OS 2.04 or newer, explicitly provide a 68000/010 build, and allow
the GTLayout interface with `gtlayout.library` V40 or newer. The test used
V45.1. Miami's supported SANA-II Ethernet path was configured for
`DEVS:Networks/a2065.device`, unit 0.

Miami started successfully without GUI for the automated runs. `ISONLINE`
returned 1. The native routing table showed interface `mi0`, the configured
IPv4 address was `10.0.2.15/24`, and the default gateway was `10.0.2.2`.
`MiamiPing` reached `10.0.2.2` with zero packet loss and `MiamiResolve`
resolved `example.com`, qualifying both native IPv4 and DNS controls.

## Deterministic SNTP and stability

The primary fixture listened on the host at `0.0.0.0:40123`, reachable from
the guest as `10.0.2.2:40123`. It received a 48-byte request and returned a
48-byte mode-4, version-4, stratum-2 response. The response copied request
bytes 40..47 exactly into bytes 24..31. The observed originate timestamp
therefore matched exactly.

The deterministic response contained NTP seconds `4000000000` and fraction
`1073741824`. AmiNTP reported Amiga seconds `1538550400` and microseconds
`250000`, exactly matching the expected conversion. The initial numeric QUERY
and five consecutive repeats all returned RC 0. A hostname QUERY to
`pool.ntp.org` also returned RC 0 over Miami's native DNS and UDP path.

The controlled failure cases behaved as required: a nonresponsive UDP port
returned RC 10 within the configured timeout, and the fixture's deliberately
wrong originate timestamp also returned RC 10. Every AmiNTP invocation exited;
the final task list contained Miami, Workbench, ConClip, and the status command,
but no stuck AmiNTP process. Five subsequent successful queries provide the
observable socket-cleanup control.

## Clock, RTC, and NORTC

`SYNC NORTC` returned RC 0, changed the disposable guest system clock from
`Monday 14-Sep-26 10:14:31` to `Saturday 03-Oct-26 07:06:40`, and reported
`RTC=SKIPPED`. A narrow emulator-side `battclock.resource` hook observed zero
`WriteBattClock` entries in this isolated run.

A separate clean run exercised normal `SYNC`. It returned RC 0, reported
`RTC=UPDATED`, set the same deterministic system time, and the hook observed
exactly one `WriteBattClock(1538550400)` call from the AmiNTP task.

Classification:

```text
M4_2_PLUS_MIAMI_NATIVE_STACK_QUALIFICATION_PASS
```

Detailed disposable evidence is retained at
`/tmp/m4_2_plus_miami_native.evidence`. Miami archives, ROMs, Workbench files,
drivers, generated settings, guest images, and emulator traces remain outside
Git. No AmiNTP source change was required.
