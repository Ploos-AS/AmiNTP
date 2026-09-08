# M3.3a qualification — FS-UAE / AROS CI bootstrap

M3.3a introduces an automated emulator gate on GitHub Actions before the classic AmigaOS release qualification.

## Scope of this first gate

The workflow installs FS-UAE and Xvfb on Ubuntu 24.04, starts an A1200-class configuration with FS-UAE's internal AROS Kickstart replacement, and requires the emulator process to remain alive for 20 seconds.

PASS evidence is written to:

```text
build/fs-uae/result.txt
```

with the contract:

```text
STATUS=PASS
GATE=FS_UAE_AROS_BOOT_SMOKE
KICKSTART=internal
MODEL=A1200
FS_UAE_EXIT=124
OBSERVATION=emulator_remained_running_for_20_seconds
```

The workflow uploads the whole `build/fs-uae/` directory, including FS-UAE version output and emulator log.

## What this does not qualify yet

This bootstrap gate does not claim that AmiNTP executed inside AROS. It does not yet qualify RexxMast, the public `AMINTP` port, Rexx RC/RESULT, QUERY/SYNC, bsdsocket.library, timer.device, battclock.resource, RTC persistence, or networking.

Those become the next M3.3a increment: a bootable guest test harness that starts AmiNTP and emits machine-readable ARexx qualification evidence back to the host.

M3.3b remains the release gate on classic AmigaOS 2.04+ with the intended TCP/IP stacks.
