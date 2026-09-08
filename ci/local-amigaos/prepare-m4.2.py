#!/usr/bin/env python3
"""Prepare an isolated, read-only-system AmiTCP probe. Does not launch FS-UAE.
Run from the repository root after the native build. Use a NEW output directory.
No OS, ROM, or stack distribution files are copied. Inspect evidence manually.
"""
import argparse
import hashlib
from pathlib import Path
import shutil

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--workbench', type=Path, required=True)
p.add_argument('--net', type=Path, required=True)
p.add_argument('--rom', type=Path, required=True)
p.add_argument('--machine', default='A1200/020',
               help='FS-UAE machine model, e.g. A4000')
p.add_argument('--cpu', default='', help='optional FS-UAE CPU override')
p.add_argument('--driver', type=Path,
               help='local extracted a2065.device; never copied into the repo')
p.add_argument('--probe', type=Path,
               help='optional native SANA-II probe executable')
p.add_argument('--probe-command', default='open',
               choices=('open', 'devicequery', 'station', 'config', 'online'))
p.add_argument('--lance', type=Path,
               help='optional Lance-Test executable, run before AmiTCP starts')
p.add_argument('--probe-only', action='store_true',
               help='stop after the optional diagnostic probe')
p.add_argument('--ipv4', required=True, help='Current public NTP server IPv4 address')
p.add_argument('--out', type=Path, required=True)
p.add_argument('--bsdsocket', action='store_true', help='qualify FS-UAE bsdsocket.library')
a = p.parse_args()
import ipaddress
ipaddress.IPv4Address(a.ipv4)
required = (a.workbench, a.net, a.rom, Path('AmiNTP'))
if not a.bsdsocket and not a.driver:
    p.error('--driver is required unless --bsdsocket is selected')
if a.driver:
    required += (a.driver,)
for path in required:
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
if not a.bsdsocket:
    shutil.copyfile(a.workbench / 'Storage/NetInterfaces/A2065', guest / 'A2065')
    (guest / 'Networks').mkdir()
    shutil.copyfile(a.driver, guest / 'Networks/a2065.device')
if a.probe:
    if not a.probe.exists():
        p.error(f'Missing {a.probe}')
    shutil.copyfile(a.probe, guest / 'sana2-probe')
if a.lance:
    if not a.lance.exists():
        p.error(f'Missing {a.lance}')
    shutil.copyfile(a.lance, guest / 'Lance-Test')
if a.driver:
    (guest / 'driver-sha256.txt').write_text(
        hashlib.sha256(a.driver.read_bytes()).hexdigest() + '  a2065.device\n')
(guest / 'env/AmiNTP/AmiNTP.conf').write_text(
    'SERVER=pool.ntp.org\nPORT=123\nTIMEOUT=5\nRETRIES=2\n')
(out / 'qualification.fs-uae').write_text(f'''[fs-uae]
amiga_model = {a.machine}
chip_memory = 2048
fast_memory = 8192
fpu = 0
{("cpu = " + a.cpu) if a.cpu else ""}
bsdsocket_library = {1 if a.bsdsocket else 0}
{('network_card = a2065\na2065 = slirp' if not a.bsdsocket else '')}
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
s = f"""DH1:C/Assign C: DH1:C
C:Assign Q: DH0:
C:Assign SYS: DH1:
C:Assign LIBS: DH1:Libs
C:Assign LOCALE: DH1:Locale
C:Assign DEVS: DH1:Devs
C:Assign REXX: Q:
C:Assign T: RAM:
{('C:Assign AmiTCP: Net:AmiTCP' if not a.bsdsocket else '')}
C:Assign ENVARC: Q:env
C:Path C: SYS:Rexxc SYS:System ADD
C:FailAt 21
"""
if not a.bsdsocket:
    s += f"""C:If EXISTS Q:Lance-Test
Q:Lance-Test diags >Q:lance.txt
C:Echo $RC >Q:lance-rc.txt
C:EndIf
C:If EXISTS Q:sana2-probe
C:Run >Q:probe-run.txt Q:sana2-probe {a.probe_command} >Q:probe-{a.probe_command}.txt
C:Echo $RC >Q:probe-{a.probe_command}-rc.txt
C:Wait 1
C:EndIf
"""
s += """C:Version >Q:os-version.txt
C:Version exec.library >>Q:os-version.txt
C:Version dos.library >>Q:os-version.txt
C:Date >Q:time-before.txt
C:Version LIBS:bsdsocket.library FILE FULL >Q:socket-file.txt
C:Version bsdsocket.library FULL >Q:socket-open.txt
"""
if not a.bsdsocket:
    s += """C:GetNetStatus >Q:net-start.txt
C:Echo $RC >Q:net-start-rc.txt
C:ShowNetStatus >Q:net-status.txt
C:netstat >Q:interfaces-routes.txt
C:Assign DEVS:Networks Q:Networks
"""
if not a.probe_only and not a.bsdsocket:
    s += """C:AddNetInterface Q:A2065 >Q:network-add.txt
C:Echo $RC >Q:network-add-rc.txt
C:ShowNetStatus >Q:network-status-after-add.txt
C:netstat >Q:interfaces-routes-after-add.txt
"""
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
s = s.replace('{a.probe_command}', a.probe_command)
(guest / 'S/Startup-Sequence').write_text(s)
(guest / 'resident').write_text('Q:AmiNTP RESIDENT >Q:resident-output.txt\nC:Echo $RC >Q:resident-rc.txt\n')
print(out / 'qualification.fs-uae')
