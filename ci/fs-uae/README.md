# FS-UAE / AROS CI harness

This directory contains the M3.3a runtime gates for AmiNTP.

## Gate 1: internal AROS Kickstart smoke

The first smoke configuration deliberately uses FS-UAE's built-in AROS replacement ROM:

```ini
kickstart_file = internal
```

`run-aros-smoke.sh` starts an A1200-class machine under Xvfb and requires FS-UAE to remain alive for 20 seconds. A timeout exit code of 124 is the expected PASS condition.

## Gate 2: pinned AROS m68k boot media

`fetch-aros-boot.sh` downloads the dated AROS m68k boot-floppy archive `AROS-20260829-amiga-m68k-boot-floppy.lha` from the official SourceForge nightly2 area, records its SHA-256 in the qualification artifact, extracts the ADF, and normalizes it to `build/fs-uae/aros-boot/bootdisk.adf`.

`run-aros-media-smoke.sh` boots FS-UAE with both `kickstart_file = internal` and that dated boot floppy. It requires the emulator to remain alive for 25 seconds and writes a separate machine-readable result.

The dated SourceForge path is pinned; the downloaded archive digest is captured on every run so provenance is visible in the artifact. A future hardening step can freeze that observed SHA-256 as an explicit allow-list once the artifact has been qualified.

These gates prove the GitHub runner can launch FS-UAE with AROS and boot controlled AROS media. They still do **not** claim that the AmiNTP binary, RexxMast, the `AMINTP` ARexx port, networking, timer.device, or battclock.resource have been exercised inside the guest. Those require the next guest-harness/native-binary gate.

Machine-readable evidence is written below `build/fs-uae/`; the workflow uploads the complete directory as an artifact.
