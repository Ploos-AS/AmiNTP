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

## M3 — ARexx and native runtime qualification

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

Qualification split:

- M3.3a-c: GitHub-hosted FS-UAE/AROS qualification for emulator boot, native Bebbo 68000 build and real guest CLI execution.
- M3.4: local FS-UAE or real-AmigaOS qualification for the actual ARexx message path with RexxMast and public port `AMINTP`.
- AROS is not treated as proof of ARexx compatibility; M3.4 runs on AmigaOS.

## M4 — Integration

### M4.1 — Configuration and startup

- `ENVARC:AmiNTP/AmiNTP.conf`
- strict SERVER/PORT/TIMEOUT/RETRIES defaults
- CLI-over-config precedence
- startup examples
- host/static regression and native-build integration

### M4.2 qualification split

- **M4.2a — FS-UAE `bsdsocket.library`: PASS.** Numeric SNTP QUERY, originate validation, timeout and malformed-origin paths pass under visible FS-UAE. The qualification also documents the Bebbo GCC 6.5.0b timestamp-compare code-generation defect and the source-level workaround used by AmiNTP. See `docs/M4_2A_BSDSOCKET_QUALIFICATION.md`.
- **M4.2b — AmiTCP_NG/SANA-II: PASS.** An A2000-compatible 68000/KS2.04/WB2.1 FS-UAE profile with native AmiTCP_NG 4.1.5, A2065/SANA-II, A2065/SLIRP and `bsdsocket_library = 0` passes DHCP, IPv4, DNS, native UDP QUERY, protocol validation, clock update, NORTC, RTC, failure paths and 5/5 repeat queries. See `docs/M4_2B_AMITCP_QUALIFICATION.md`.

## M4.2+ — TCP/IP stacks and runtime matrix

- **AmiTCP qualification: PASS.** AmiTCP_NG 4.1.5 is qualified on the native A2065/SANA-II path with FS-UAE socket emulation disabled.
- **Miami qualification: PASS.** Miami 3.2b2 on the same native 68000/KS2.04/WB2.1 A2065/SLIRP platform passes IPv4, DNS, deterministic QUERY, clock update, RTC/NORTC, failure paths and 5/5 stability with `bsdsocket_library = 0`. See `docs/M4_2_PLUS_MIAMI_QUALIFICATION.md`.
- **Roadshow qualification: deferred.** Roadshow remains a compatibility target after the 1.0.0 release and is not claimed as qualified for 1.0.0.
- native runtime regression matrix can be extended after release without blocking 1.0.0.

## M5 — Release

Target release:

- **v1.0.0**

Release requirements:

- release qualification: complete for the claimed AmiTCP/Miami matrix
- documentation: release-ready
- Aminet packaging: automated by GitHub Actions
- `AmiNTP.lha` and sibling `AmiNTP.readme`: generated automatically
- SHA-256 checksums: generated automatically
- GitHub Release: created automatically on `v1.0.0` tag
- copyright: Ploos AS
- Aminet uploader: Per Gustav Ousdal <amiga@ousdal.org>

Roadshow qualification is explicitly post-1.0.0 and does not block this release.
