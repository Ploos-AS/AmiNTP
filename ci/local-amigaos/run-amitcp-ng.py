#!/usr/bin/env python3
"""Run one disposable AmiTCP_NG FS-UAE configuration and collect markers."""
import argparse, os, signal, subprocess, time
from pathlib import Path

p = argparse.ArgumentParser()
p.add_argument('--config', type=Path, required=True)
p.add_argument('--run-root', type=Path, required=True)
p.add_argument('--timeout', type=float, default=120)
a = p.parse_args()
if a.run_root.exists():
    raise SystemExit(f'run root already exists: {a.run_root}')
a.run_root.mkdir(parents=True)
out = a.run_root/'qualification/output'
log = (a.run_root/'host.log').open('w')
start = time.monotonic()
proc = subprocess.Popen(['fs-uae', str(a.config.resolve())], stdout=log,
                        stderr=subprocess.STDOUT, start_new_session=True)
seen = {}
try:
    while time.monotonic() - start < a.timeout:
        for f in out.glob('*'):
            if not f.name.endswith('.uaem') and f.name not in seen:
                seen[f.name] = time.monotonic() - start
        if '99_DONE' in seen:
            break
        time.sleep(.1)
finally:
    if proc.poll() is None:
        os.killpg(proc.pid, signal.SIGTERM)
        try: proc.wait(5)
        except subprocess.TimeoutExpired:
            os.killpg(proc.pid, signal.SIGKILL); proc.wait()
    log.close()
status = 'PASS' if '99_DONE' in seen else 'TIMEOUT'
(a.run_root/'summary.txt').write_text(
    f'status={status}\npid={proc.pid}\n' +
    ''.join(f'{k}={v:.3f}s\n' for k, v in sorted(seen.items())))
print(status)
raise SystemExit(0 if status == 'PASS' else 1)
