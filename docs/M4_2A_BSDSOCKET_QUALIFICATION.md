# M4.2a FS-UAE bsdsocket.library qualification

## Status: BLOCKED (2026-09-08)

This is a separate qualification track from the native AmiTCP/SANA-II track in
[`M4_2_QUALIFICATION.md`](M4_2_QUALIFICATION.md). It uses real AmigaOS under
FS-UAE with `bsdsocket_library = 1`; it does not use AmiTCP, A2065, SANA-II, or
host-side mock networking.

### Environment

- FS-UAE 3.2.35, A1200/020, 2 MiB chip and 8 MiB fast RAM
- AmigaOS 3.1, Kickstart 40.68, Workbench 40.42
- FS-UAE `bsdsocket_library = 1`; no `network_card` or A2065 configuration
- Guest `bsdsocket.library` reports 4.1; the Workbench library identifies as AmiTCP_NG 4.1.5
- The harness does not assign or start AmiTCP

### Attempt

The disposable harness was generated with `prepare-m4.2.py --bsdsocket` and a
fresh Bebbo GCC 6.5.0b `-m68000 -mcrt=nix20` binary (SHA-256:
`e73f0f7e46fa3fa5da0b0be9610eb5452dd15f73d7e1c696216dc28cf5682810`). The
startup sequence successfully recorded the OS and socket-library versions, but
the guest stopped progressing when executing `AmiNTP VERSION`: `version.txt`
was created empty and no return-code file was produced before the watchdog
terminated FS-UAE. Consequently no DNS, UDP, configuration, SYNC, or ARexx
runtime result was observed. This is an execution/harness blocker, not a PASS.

Host/static validation remains required and is recorded in the final
qualification report. The native AmiTCP/SANA-II A2065 track remains M4.2b and
BLOCKED by the previously documented A2065/device boundary.

## Startup-boundary investigation

A fresh native binary was tested in disposable minimal profiles. Both
`bsdsocket_library = 0` and `= 1` reached the `BEFORE` marker and created the
redirected output file, but the output contained only:

```
locale.library failed to load
```

No RC or `AFTER` marker was produced. The same failure was reproduced before
any socket-library version command, DNS operation, RexxMast startup, or AmiTCP
assignment. Thus the prior conclusion that enabling bsdsocket alone changes
AmiNTP execution is not supported: the minimal control profile fails at the
same native/libnix startup boundary. The next required step is to reproduce the
known M3.4 startup environment (including its complete system assignments and
locale setup) and isolate the `locale.library` load failure before retrying
M4.2a.
