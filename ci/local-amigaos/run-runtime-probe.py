#!/usr/bin/env python3
"""Run one disposable FS-UAE runtime probe with owned lifecycle and markers."""
import argparse, os, shutil, signal, subprocess, time
from pathlib import Path

MARKERS = ('00_BOOT_START','10_LIBS_READY','20_BEFORE','E1_MAIN','30_RC_','40_AFTER','99_DONE')
p = argparse.ArgumentParser()
p.add_argument('--workbench', type=Path, required=True)
p.add_argument('--rom', type=Path, required=True)
p.add_argument('--executable', type=Path, required=True)
p.add_argument('--out-root', type=Path, required=True)
p.add_argument('--machine', default='A1200/020')
p.add_argument('--bsdsocket', action='store_true')
p.add_argument('--timeout', type=float, default=120.0)
p.add_argument('--run-id', required=True)
p.add_argument('--no-e1-marker', action='store_true', help='classify by RC/DONE without E1_MAIN')
p.add_argument('--system-assigns', action='store_true', help='use the M3.4-style Workbench system assigns')
p.add_argument('--args', default='', help='arguments passed to the guest executable')
a = p.parse_args()
if not a.executable.is_file(): p.error(f'missing executable: {a.executable}')
run = (a.out_root / a.run_id).resolve(); guest = run / 'Qualification'; output = guest / 'output'
if run.exists(): p.error(f'run directory already exists: {run}')
(output).mkdir(parents=True); (guest/'S').mkdir()
shutil.copyfile(a.executable, guest/a.executable.name)
for f in output.iterdir():
    if f.name.startswith(('00_','10_','20_','30_','40_','99_','E1_')): p.error('stale marker')
assigns = ""
if a.system_assigns:
    assigns = """DH1:C/Assign SYS: DH1:
DH1:C/Assign LIBS: SYS:Libs
DH1:C/Assign DEVS: SYS:Devs
DH1:C/Assign L: SYS:L
DH1:C/Assign FONTS: SYS:Fonts
DH1:C/Assign S: SYS:S
DH1:C/Assign REXX: SYS:Rexxc
DH1:C/Assign T: RAM:
DH1:C/Path C: SYS:Rexxc SYS:System ADD
DH1:C/Stack 8192
"""
else:
    assigns = 'DH1:C/Assign LIBS: DH1:Libs\n'
startup = f'''C:Echo BOOT_START >DH0:output/00_BOOT_START
{assigns}'''
startup += f'''C:Echo LIBS_READY >DH0:output/10_LIBS_READY
C:Echo BEFORE >DH0:output/20_BEFORE
DH0:{a.executable.name} {a.args} >DH0:output/program.txt
C:Echo $RC >DH0:output/30_RC_$RC
C:Echo AFTER >DH0:output/40_AFTER
C:Echo DONE >DH0:output/99_DONE
'''
(guest/'S/Startup-Sequence').write_text(startup)
conf = f'''[fs-uae]\namiga_model = {a.machine}\nchip_memory = 2048\nfast_memory = 8192\nbsdsocket_library = {1 if a.bsdsocket else 0}\nkickstart_file = {a.rom.resolve()}\nhard_drive_0 = {guest}\nhard_drive_0_label = Qualification\nhard_drive_0_priority = 10\nhard_drive_1 = {a.workbench.resolve()}\nhard_drive_1_label = Workbench\nhard_drive_1_read_only = 1\nhard_drive_1_priority = 0\nbase_dir = {run/'fs-uae'}\nlogs_dir = {run/'logs'}\nfullscreen = 0\nsound_output = 0\nautomatic_input_grab = 0\n'''
(run/'fs-uae.conf').write_text(conf)
start = time.monotonic(); log = (run/'emulator.log').open('w')
proc = subprocess.Popen(['fs-uae', str(run/'fs-uae.conf')], stdout=log, stderr=subprocess.STDOUT, start_new_session=True)
(run/'pid').write_text(str(proc.pid))
seen = {}; status = 'TIMEOUT'
try:
    while time.monotonic()-start < a.timeout:
        for f in output.iterdir():
            if f.name not in seen and not f.name.endswith('.uaem') and (f.name.startswith(('00_','10_','20_','30_','40_','99_','E1_'))): seen[f.name]=time.monotonic()-start
        if '99_DONE' in seen:
            status='PASS' if any(k.startswith('30_RC_0') for k in seen) and (a.no_e1_marker or 'E1_MAIN' in seen) else 'GUEST_FAIL'; break
        time.sleep(.1)
finally:
    if proc.poll() is None:
        os.killpg(proc.pid, signal.SIGTERM)
        try: proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            os.killpg(proc.pid, signal.SIGKILL); proc.wait()
    log.close()
(run/'summary.txt').write_text(f'status={status}\npid={proc.pid}\n' + ''.join(f'{k}={v:.3f}s\n' for k,v in sorted(seen.items())))
print(f'{status} {run}')
raise SystemExit(0 if status=='PASS' else 1)
