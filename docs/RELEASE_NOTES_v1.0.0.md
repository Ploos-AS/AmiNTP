# AmiNTP 1.0.0

AmiNTP 1.0.0 is the first public release of the native SNTP client for classic Amiga systems.

## Highlights

- AmigaOS 2.04+ support
- Motorola 68000 minimum CPU
- no FPU requirement
- no GUI dependency
- SNTP queries over UDP
- DNS names and numeric IPv4 targets
- query-only mode
- system clock synchronization
- optional RTC update
- `NORTC` mode
- configuration through `ENVARC:AmiNTP/AmiNTP.conf`
- ARexx control through the `AMINTP` public port
- bounded timeout/retry handling
- strict SNTP response validation, including originate timestamp validation

## Qualified native TCP/IP stacks

The 1.0.0 release has completed deterministic native-stack qualification with:

- AmiTCP_NG 4.1.5
- Miami 3.2b2

The native qualification platform used a Motorola 68000, Kickstart 2.04, Workbench 2.1 and A2065/SANA-II networking under FS-UAE with FS-UAE socket emulation disabled.

Qualification covered IPv4, DNS, numeric and hostname queries, timestamp conversion, system clock updates, RTC/NORTC behavior, timeout handling, malformed-origin rejection and repeated successful query cycles.

Roadshow remains a compatibility target but is not claimed as qualified in 1.0.0.

## Distribution

The release workflow publishes:

- `AmiNTP.lha` — Aminet-compatible archive
- `AmiNTP.readme` — sibling Aminet metadata file
- `AmiNTP.lha.sha256`
- `AmiNTP-v1.0.0.zip`
- `AmiNTP-v1.0.0.zip.sha256`

Copyright (c) 2026 Ploos AS.

Aminet uploader: Per Gustav Ousdal <amiga@ousdal.org>

License: MIT.
