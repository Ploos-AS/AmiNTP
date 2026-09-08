#!/usr/bin/env python3
"""Prepare an isolated, read-only-system AmiTCP probe. Does not launch FS-UAE.
Run from the repository root after the native build. Use a NEW output directory.
No OS, ROM, or stack distribution files are copied. Inspect evidence manually.
"""
import argparse
from pathlib import Path
import shutil

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--workbench', type=Path, required=True)
p.add_argument('--net', type=Path, required=True)
p.add_argument('--rom', type=Path, required=True)
p.add_argument('--ipv4', required=True, help='Current public NTP server IPv4 address')
p.add_argument('--out', type=Path, required=True)
a = p.parse_args()
import ipaddress
ipaddress.IPv4Address(a.ipv4)
for path in (a.workbench, a.net, a.rom, Path('AmiNTP')):
    if not path.exists():
        p.error(f'Missing {path}')
a.out.mkdir(parents=True, exist_ok=False)
out = a.out.resolve()
guest = out / 'guest'
(guest / 'S').mkdir(parents=True)
(guest / 'env/AmiNTP').mkdir(parents=True)
shutil.copyfile('AmiNTP', guest / 'AmiNTP')
shutil.copyfile('ci/local-amigaos/m4.2-arexx.rexx', guest / 'm4.2-arexx.rexx')
shutil.copyfile('docs/evidence/m3.4/ports.rexx', guest / 'ports.rexx')
(guest / 'env/AmiNTP/AmiNTP.conf').write_text(
    'SERVER=pool.ntp.org\nPORT=123\nTIMEOUT=5\nRETRIES=2\n')
(out / 'qualification.fs-uae').write_text(f'''[fs-uae]
amiga_model = A1200/020
chip_memory = 2048
fast_memory = 8192
fpu = 0
bsdsocket_library = 0
kickstart_file = {a.rom.resolve()}
hard_drive_0 = {guest}
hard_drive_0_label = Qualification
hard_drive_0_priority = 10
hard_drive_1 = {a.workbench.resolve()}
hard_drive_1_label = Workbench
hard_drive_1_read_only = 1
hard_drive_1_priority = 0
hard_drive_2 = {a.net.resolve()}
hard_drive_2_label = Net
hard_drive_2_read_only = 1
base_dir = {out}/fs-uae
logs_dir = {out}/logs
fullscreen = 0
sound_output = 0
automatic_input_grab = 0
''')
s = '''DH1:C/Assign C: DH1:C
C:Assign Q: DH0:
C:Assign SYS: DH1:
C:Assign LIBS: DH1:Libs
C:Assign DEVS: DH1:Devs
C:Assign REXX: Q:
C:Assign T: RAM:
C:Assign AmiTCP: Net:AmiTCP
C:Assign ENVARC: Q:env
C:Path C: SYS:Rexxc SYS:System ADD
C:FailAt 21
C:Version >Q:os-version.txt
C:Version exec.library >>Q:os-version.txt
C:Version dos.library >>Q:os-version.txt
C:Date >Q:time-before.txt
C:Version LIBS:bsdsocket.library FILE FULL >Q:socket-file.txt
C:GetNetStatus >Q:net-start.txt
C:Echo $RC >Q:net-start-rc.txt
C:Version bsdsocket.library FULL >Q:socket-open.txt
C:ShowNetStatus >Q:net-status.txt
C:netstat >Q:interfaces-routes.txt
'''
commands = [('version', 'VERSION'),
            ('dns', 'QUERY SERVER=pool.ntp.org TIMEOUT=5 RETRIES=2'),
            ('literal', f'QUERY SERVER={a.ipv4} TIMEOUT=5 RETRIES=2'),
            ('failure', 'QUERY SERVER=192.0.2.1 TIMEOUT=5 RETRIES=2'),
            ('loopback', 'QUERY SERVER=127.0.0.1 PORT=49123 TIMEOUT=5 RETRIES=2'),
            ('config', 'QUERY'),
            ('override', f'QUERY SERVER={a.ipv4}')]
for name, command in commands:
    s += f'C:Echo "AmiNTP {command}" >Q:{name}-command.txt\n'
    s += f'C:Date >Q:{name}-before.txt\nQ:AmiNTP {command} >Q:{name}.txt\n'
    s += f'C:Set {name}rc $RC\nC:Echo ${name}rc >Q:{name}-rc.txt\nC:Date >Q:{name}-after.txt\n'
s += f'''If $dnsrc EQ 0
If $literalrc EQ 0
If $configrc EQ 0
If $overriderc EQ 0
C:Echo ALLOWED >Q:allow-sync
C:Date >Q:sync-before.txt
Q:AmiNTP SYNC SERVER={a.ipv4} NORTC >Q:sync.txt
C:Echo $RC >Q:sync-rc.txt
C:Date >Q:sync-after.txt
EndIf
EndIf
EndIf
EndIf
SYS:System/RexxMast >Q:rexxmast.txt
C:Wait 2
SYS:Rexxc/RX Q:ports.rexx >Q:ports-before.txt
C:Run >Q:run.txt C:Execute Q:resident
C:Wait 2
SYS:Rexxc/RX Q:ports.rexx >Q:ports-running.txt
SYS:Rexxc/RX Q:m4.2-arexx.rexx pool.ntp.org >Q:arexx.txt
C:Echo $RC >Q:harness-rc.txt
C:Wait 2
SYS:Rexxc/RX Q:ports.rexx >Q:ports-after.txt
C:Copy RAM:AmiTCP.log Q:stack-log.txt
C:Echo DONE >Q:done.txt
'''
(guest / 'S/Startup-Sequence').write_text(s)
(guest / 'resident').write_text('Q:AmiNTP RESIDENT >Q:resident-output.txt\nC:Echo $RC >Q:resident-rc.txt\n')
print(out / 'qualification.fs-uae')
