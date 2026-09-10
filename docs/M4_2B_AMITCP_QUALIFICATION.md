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

Only after the interface reaches an online state can socket, raw UDP, AmiNTP,
SYNC NORTC, and ARexx network tests be qualified. M4.2a remains separate.
