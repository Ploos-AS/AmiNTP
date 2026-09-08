# M3.4 Local AmigaOS ARexx qualification

M3.4 is intentionally a local qualification gate. GitHub-hosted CI uses AROS to qualify the native m68k build and CLI/runtime path, but ARexx is qualified on a real AmigaOS installation with RexxMast rather than emulated with a non-equivalent AROS substitute.

## Required environment

- FS-UAE or real Amiga hardware running AmigaOS 2.04 or newer.
- RexxMast active.
- The native Bebbo-built `AmiNTP` executable from the same commit being qualified.
- Enough shell access for Codex/local automation to copy files, start AmiNTP and capture output.

## Procedure

1. Copy `AmiNTP` and `ci/local-amigaos/m3.4-arexx.rexx` into the guest.
2. Confirm RexxMast is running.
3. Start `AmiNTP RESIDENT` in the background and confirm the process remains resident.
4. Run the ARexx script with the system `RX` command.
5. Capture the complete output as qualification evidence.
6. PASS requires all tested commands to return RC 0 and the script to finish with:

   `STATUS=PASS`
   `GATE=M3_4_LOCAL_AMIGAOS_AREXX`

The harness tests `PING`, `VERSION`, `STATUS`, `LASTSYNC`, `LASTERROR`, and `QUIT`. `QUERY` and `SYNC` remain separate network/clock integration tests because they require a configured TCP/IP stack and can mutate system/RTC time.

## Evidence to record

Record the AmiNTP commit SHA, binary SHA-256, Amiga model/CPU, AmigaOS version, FS-UAE version when applicable, RexxMast presence, the exact RX output, and whether the worktree was clean before and after qualification.

Do not mark M3.4 PASS from host unit tests alone. M3.2 host tests validate dispatcher logic; M3.4 proves the actual `AMINTP` public port and real ARexx message path on AmigaOS.
