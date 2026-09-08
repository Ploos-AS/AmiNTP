# AmiNTP roadmap

## Product contract

AmiNTP is a small native SNTP client for classic Amiga systems.

Baseline:

- AmigaOS 2.04+
- Motorola 68000 minimum
- no FPU requirement
- no GUI
- native executable
- bsdsocket.library-compatible TCP/IP
- Bebbo GCC
- ARexx control in resident mode
- ARexx port name: `AMINTP`

## M0 — Foundation

- repository structure
- MIT license
- explicit platform contract
- Bebbo GCC Makefile
- explicit `-m68000` target
- CLI skeleton
- version/help output
- stable initial return-code policy
- static qualification checks

## M1 — SNTP core

- IPv4
- DNS name resolution
- UDP/123
- 48-byte NTP packet encode/decode
- client request timestamp
- response validation
- stratum and leap checks
- timeout
- bounded retry
- query mode without clock modification

## M2 — Clock

- convert NTP epoch to Amiga time
- set system clock
- optional RTC update
- no-RTC mode
- large-offset sanity checks
- stable success/failure reporting

## M3 — ARexx

Resident mode with public port `AMINTP`.

Initial commands:

- PING
- VERSION
- STATUS
- SERVER
- QUERY
- SYNC
- LASTSYNC
- LASTERROR
- QUIT

Results must be stable and machine-readable.

## M4 — Integration

- configuration file
- startup examples
- AmiTCP qualification
- Miami qualification
- Roadshow qualification
- native runtime regression matrix

## M5 — Release

- release qualification
- documentation
- Aminet packaging
- checksums
- v1.0.0
