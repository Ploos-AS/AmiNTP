# AmiNTP

AmiNTP is a small native SNTP client for classic Amiga systems.

The project targets AmigaOS 2.04 and later, with a Motorola 68000 minimum CPU,
no FPU requirement, no GUI dependencies, and a bsdsocket.library-compatible
TCP/IP stack.

## Project goals

- Native Amiga executable
- AmigaOS 2.04+
- Motorola 68000 minimum
- No FPU required
- No GUI
- SNTP over UDP/123
- DNS names and IPv4 addresses
- System clock synchronization
- Optional RTC update
- Stable CLI interface
- ARexx control through an `AMINTP` port
- AmiTCP, Miami and Roadshow compatibility targets
- Bebbo GCC toolchain

AmiNTP is intentionally not a full NTP daemon. Its primary job is to obtain
correct time reliably and expose that functionality through the CLI and ARexx.

## Current status

M0 foundation is implemented.

The current binary is a CLI skeleton only. Network synchronization, clock
setting and the ARexx port are introduced by later milestones.

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

## M0 CLI

```text
AmiNTP ?
AmiNTP HELP
AmiNTP VERSION
AmiNTP SERVER=pool.ntp.org
```

M0 parses the basic command line but deliberately does not contact an NTP
server yet.

## Planned milestones

- **M0 — Foundation:** build, platform contract, CLI skeleton, version/help
- **M1 — SNTP core:** UDP, DNS, packet encode/decode, query, timeout/retry
- **M2 — Clock:** system clock, RTC, sync and sanity checks
- **M3 — ARexx:** `AMINTP` port and stable command/result API
- **M4 — Integration:** configuration and TCP/IP stack qualification
- **M5 — Release:** native runtime qualification and Aminet packaging

See [docs/ROADMAP.md](docs/ROADMAP.md) for details.

## License

MIT. See [LICENSE](LICENSE).
