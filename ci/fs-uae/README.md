# FS-UAE / AROS CI harness

This directory contains the first M3.3a runtime gate for AmiNTP.

The smoke configuration deliberately uses FS-UAE's built-in AROS replacement ROM:

```ini
kickstart_file = internal
```

`run-aros-smoke.sh` starts an A1200-class machine under Xvfb and requires FS-UAE to remain alive for 20 seconds. A timeout exit code of 124 is therefore the expected PASS condition for this first boot gate.

This gate proves only that the GitHub runner can launch FS-UAE with the internal AROS Kickstart replacement and keep the emulator running. It does **not** yet claim that AmiNTP, RexxMast, the `AMINTP` ARexx port, networking, timer.device, or battclock.resource have been exercised inside the guest.

Machine-readable output is written to `build/fs-uae/result.txt`; the workflow uploads the complete result directory as an artifact.
