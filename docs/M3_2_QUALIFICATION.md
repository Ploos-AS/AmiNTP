# M3.2 qualification — ARexx operational commands

M3.2 extends the resident `AMINTP` ARexx host with operational commands while keeping native runtime qualification for M3.3.

Supported commands:

- `PING`
- `VERSION`
- `STATUS`
- `QUERY <server>` or `QUERY SERVER=<server>`
- `SYNC <server> [NORTC]`
- `LASTSYNC`
- `LASTERROR`
- `QUIT`

`QUERY` never changes clocks. `SYNC` applies system time and writes RTC unless `NORTC` is supplied. `LASTSYNC` records the latest completed synchronization result. `LASTERROR` records the latest command error. `QUIT` returns `OK QUIT` and requests an orderly resident-loop shutdown after replying to the Rexx message.

Return-code policy remains: 0 success, 5 warning/partial success, 10 network/protocol/unknown command failure, 20 usage/internal failure.

Host qualification:

```sh
make m3.2-check
```

The test uses mocked query/sync operations and verifies QUERY, SYNC, NORTC, LASTSYNC, LASTERROR, RTC-warning semantics and QUIT state without changing the host clock.

Native/runtime items remain pending for M3.3: Bebbo `-m68000` compile of `arexx_amiga.c`, `rexxsyslib.library` V36 open, public `AMINTP` message port, Rexx `RC`/`RESULT`, network-backed QUERY/SYNC, RTC behavior and orderly QUIT in FS-UAE.
