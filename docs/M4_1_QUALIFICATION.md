# M4.1 configuration and startup qualification

## Scope

M4.1 introduces the first integration layer for normal AmigaOS use:

- default configuration path `ENVARC:AmiNTP/AmiNTP.conf`
- strict `KEY=VALUE` configuration parser
- keys `SERVER`, `PORT`, `TIMEOUT`, `RETRIES`
- CLI values override configuration values
- a missing default configuration file is allowed
- malformed values and unknown keys fail with RC 20
- startup examples for `S:User-Startup`
- native Bebbo build includes the configuration module

Configuration is intentionally limited to connection defaults. `QUERY`, `SYNC`, `NORTC` and `RESIDENT` remain explicit operational choices.

## Validation contract

`make m4.1-check` must demonstrate:

1. the complete existing M0-M3 host/static regression remains green;
2. valid configuration values are loaded;
3. CLI values have precedence over configuration values;
4. a missing configuration file is non-fatal;
5. invalid numeric configuration is rejected with RC 20;
6. CLI port overflow and malformed numeric input are rejected;
7. the canonical AmigaOS configuration path and example file are present.

The FS-UAE/AROS workflow must also continue to build and execute the native Bebbo `-m68000 -mcrt=nix20` binary after `src/config.c` is added.

## Runtime integration remaining after M4.1

M4.1 does not claim TCP/IP-stack qualification. AmiTCP, Miami and Roadshow remain separate M4 qualification gates, along with the native runtime matrix.
