# AmiNTP

AmiNTP is a small native SNTP client for classic Amiga systems.

The project targets AmigaOS 2.04 and later, with a Motorola 68000 minimum CPU,
no FPU requirement, no GUI dependencies, and a bsdsocket.library-compatible
TCP/IP stack.

## Features

- Native Amiga executable
- AmigaOS 2.04+
- Motorola 68000 minimum
- No FPU required
- No GUI
- SNTP over UDP
- DNS names and IPv4 addresses
- Query mode without changing the clock
- System clock synchronization
- Optional RTC update
- `NORTC` mode
- Configuration via `ENVARC:AmiNTP/AmiNTP.conf`
- Stable CLI return codes
- ARexx control through the `AMINTP` port
- Bebbo GCC toolchain

AmiNTP is intentionally not a full NTP daemon. Its primary job is to obtain
correct time reliably and expose that functionality through the CLI and ARexx.

## Current status

AmiNTP 1.0.0 is release-ready.

Native end-to-end qualification has passed on a 68000 / Kickstart 2.04 /
Workbench 2.1 configuration with:

- AmiTCP_NG 4.1.5 using native `bsdsocket.library`
- Miami 3.2b2 using native `bsdsocket.library`
- A2065/SANA-II networking under FS-UAE with socket emulation disabled

Qualification covers IPv4, DNS, numeric and hostname SNTP queries, protocol
validation, NTP-to-Amiga time conversion, system clock updates, RTC updates,
`NORTC`, timeout handling, malformed-origin rejection and repeated queries.
Roadshow remains a compatibility target for a later qualification pass and is
not claimed as qualified for 1.0.0.

## Build

A Bebbo m68k-amigaos GCC toolchain is expected:

```sh
make
```

The default compiler is:

```text
m68k-amigaos-gcc
```

Override it if needed:

```sh
make CC=/path/to/m68k-amigaos-gcc
```

The build explicitly targets the 68000:

```text
-m68000
```

## CLI

Examples:

```text
AmiNTP ?
AmiNTP HELP
AmiNTP VERSION
AmiNTP QUERY SERVER=pool.ntp.org
AmiNTP SYNC SERVER=pool.ntp.org
AmiNTP SYNC SERVER=pool.ntp.org NORTC
```

Configuration defaults can be stored in:

```text
ENVARC:AmiNTP/AmiNTP.conf
```

See `examples/AmiNTP.conf` and the qualification documents under `docs/`.

## Release

The GitHub release workflow is tag-driven. A `v1.0.0` tag builds the native
68000 binary, runs the regression checks, creates the Aminet-compatible
`AmiNTP.lha` + `AmiNTP.readme` pair, creates checksums and publishes the files
as GitHub Release assets.

## Milestones

- **M0 — Foundation:** complete
- **M1 — SNTP core:** complete
- **M2 — Clock:** complete
- **M3 — ARexx/native runtime:** complete
- **M4 — Integration:** AmiTCP and Miami native qualification complete
- **M5 — Release:** 1.0.0 release packaging ready

See [docs/ROADMAP.md](docs/ROADMAP.md) for details.

## Copyright and license

Copyright (c) 2026 Ploos AS.

MIT. See [LICENSE](LICENSE).
