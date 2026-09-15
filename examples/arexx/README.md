# AmiNTP ARexx examples

These examples are intended for AmiNTP 1.0.1 and use the public `AMINTP` ARexx port.

Prerequisites:

- RexxMast is running.
- AmiNTP has been started in resident mode and owns the `AMINTP` port.
- A working bsdsocket.library-compatible TCP/IP stack is available for network operations.

Examples:

- `ping.rexx` — verify that the resident AmiNTP ARexx port responds.
- `status.rexx` — show version, resident status, configured server, last synchronization and last error.
- `query.rexx [server]` — query a server without changing the clock.
- `sync.rexx [server]` — synchronize the clock.
- `safe-sync.rexx [server]` — QUERY first and only SYNC after a successful query.
- `startup-sync.rexx [server]` — startup-safe synchronization; failures do not abort startup.
- `diagnostics.rexx` — machine-friendly command/RC/result diagnostics.

If no server argument is supplied by the query/sync examples, `pool.ntp.org` is used.

Typical Shell use:

```text
RX examples/arexx/ping.rexx
RX examples/arexx/status.rexx
RX examples/arexx/query.rexx pool.ntp.org
RX examples/arexx/safe-sync.rexx pool.ntp.org
```

For startup automation, copy the desired script to a stable location such as `S:` or `REXX:` and invoke it after the TCP/IP stack and AmiNTP resident process have started. For example:

```text
RX S:AmiNTP-startup.rexx pool.ntp.org
```

`startup-sync.rexx` deliberately exits successfully when AmiNTP or the network is unavailable so a transient time-sync problem does not stop the rest of `S:User-Startup`.

These scripts are examples rather than a replacement for AmiNTP's normal CLI/configuration interface. They are intentionally small so they can also serve as templates for BBS software, monitoring, scheduled jobs and other Amiga automation.
