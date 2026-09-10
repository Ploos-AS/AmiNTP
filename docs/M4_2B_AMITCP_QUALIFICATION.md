# M4.2b — AmiTCP_NG native-stack qualification

This is separate from M4.2a. It uses AmiTCP_NG's own `LIBS:bsdsocket.library`
with FS-UAE's built-in `bsdsocket_library = 0`. It does not qualify the
built-in host socket emulation, AmiTCP 3.x, Miami, Roadshow, SANA-II hardware,
or a physical A2065.

## Runtime inputs

The local machine has a disposable AmiTCP_NG 4.1.5 payload and ADF. The
upstream source was inspected at commit `504cacbdb7f0912fa2d1d49cce4a90b2a0540f8c`
(`v4.1.7`). Payloads stay outside Git. The A2065 driver is also a local input.
The prepared profile is A4000/68020, 2 MiB Chip, 8 MiB Fast, Kickstart 40.68
A4000, `network_card = a2065`, `a2065 = slirp`, and `bsdsocket_library = 0`.

`ci/local-amigaos/prepare-amitcp-ng.py` creates a unique ignored runtime,
copies the local Workbench into a disposable writable system, overlays the
AmiTCP_NG install data, installs `DEVS:Networks/a2065.device`, and writes an
`eth0` DHCP interface. `ci/local-amigaos/run-amitcp-ng.py` owns one FS-UAE
process, records markers, and reaps it on completion or timeout.

## Current result

The first A4000 boot reached `BOOT_START` and `LIBS_READY`, then stopped during
the synchronous `AddNetInterface` call. No interface status, socket probe, UDP
exchange, or AmiNTP query was observed. This is a stack/device boundary
failure; no AmiNTP networking code was changed.

The local A2065 driver input is the previously tested 2.16a build
(`f0727a46b5a82f95c7a2dbdcde60a66d043ab734e76670c77aa766b498cf7855`). Earlier
valid A4000 evidence recorded malformed LANCE configuration and no usable
device initialization. AmiTCP_NG qualification is therefore **BLOCKED** pending
a working A2065 SANA-II device under this FS-UAE profile.

## M4.2b.1 — A2065/SANA-II bring-up isolation

The generated configuration is an A4000 with `network_card = a2065`,
`a2065 = slirp`, and `bsdsocket_library = 0`; FS-UAE 3.2.35 logs the emulated
`A2065 Z2 Ethernet` device. The guest interface file is:

```text
device=a2065.device
unit=0
configure=dhcp
requiresinitdelay=no
```

The driver is installed as `DEVS:Networks/a2065.device` and is readable by the
guest. The independent native SANA-II probe (SHA-256
`95a68857d82b548af7d5d029765f3f71aa736044c92680f8c94a979a87ea28f8`) reaches
`BEFORE_OPEN` and returns `OPENERR=4294967295` (`IOERR_OPENFAIL`) on the valid
A4000 profile. It does not reach `S2_DEVICEQUERY`, station-address, configure,
or online commands. This is an `OPENDEVICE_ERROR`, not a DHCP result.

The AmiTCP_NG boot then reaches `BOOT_START` and `LIBS_READY`; synchronous
`AddNetInterface eth0` does not return before the watchdog. Since the
independent probe fails at `OpenDevice`, the first proven boundary is the
A2065 driver/emulator compatibility layer (Outcome B in the isolation plan),
below AmiTCP_NG and before SANA-II or DHCP traffic. No evidence justifies an
AmiNTP or AmiTCP_NG protocol change.

Only after the interface reaches an online state can socket, raw UDP, AmiNTP,
SYNC NORTC, and ARexx network tests be qualified. M4.2a remains separate.

## M4.2b.3 — Amiberry upstream-parity attempt

The upstream-pinned container was built locally from
`docker/Dockerfile.amiberry`. It pins Amiberry v7.1.1 at source commit
`c53d6db770a5580732a7f2bb48abcbfcd95fd0f4`; the resulting local image digest
is `sha256:8b857755cf45b2fc47421c3882493a72ab1ef4040a210fc5fededb5768b50fe5`.
The run used `NET=1`, A4000, CPU 68020, A2065 SLIRP, and the emulator's own
network implementation (no FS-UAE bsdsocket shortcut).

The disposable run used the locally owned A4000 Kickstart 40.68 image and the
local Workbench tree with the 2.16a driver staged at
`DEVS:Networks/a2065.device`. A deterministic startup was installed in the
disposable copy to write boot markers, run static `AddNetInterface`, and write
status output. The guest produced no boot marker before the 90-second watchdog
in either the stock or explicit-startup run. Consequently A2065 enumeration,
OpenDevice, SANA-II, and AmiTCP_NG interface state were not observed in this
attempt. This does not contradict upstream: its harness requires an installed
AmigaOS 3.2 Workbench tree and its own prepared test files, while the local
input here is Workbench 3.1/40.42.

The Amiberry path is now prepared and buildable, but M4.2b remains **BLOCKED**
at guest boot/input parity. No AmiNTP or AmiTCP_NG networking code was changed.

## Upstream parity correction

AmiTCP_NG's documented successful A2065 qualification uses **Amiberry v7.1.1
with its in-tree SLIRP/A2065 emulation**, not FS-UAE. Its guest network is
`10.0.2.15/24`, gateway `10.0.2.2`, DNS `10.0.2.3`; the upstream documentation
explicitly says the Debian FS-UAE harness has no network card and directs real
NIC tests to Amiberry.

The earlier direct probe used bare `a2065.device`. Upstream's troubleshooting
notes state that direct `OpenDevice()` requires the full
`DEVS:Networks/a2065.device` path. The reusable probe now defaults to that path
and accepts an explicit device argument. The earlier bare-name `IOERR_OPENFAIL`
is therefore not treated as conclusive. The FS-UAE AddNetInterface hang remains
unresolved at the device boundary, with no evidence of an AmiNTP or AmiTCP_NG
protocol defect. The next valid runtime path is the upstream Amiberry Docker
harness using the locally owned ROM, Workbench, and driver inputs.
