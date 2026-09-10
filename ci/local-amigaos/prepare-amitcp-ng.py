#!/usr/bin/env python3
"""Prepare an ignored, disposable A4000/A2065 AmiTCP_NG FS-UAE runtime."""
import argparse, hashlib, shutil
from pathlib import Path

p = argparse.ArgumentParser()
p.add_argument('--workbench', type=Path, required=True)
p.add_argument('--rom', type=Path, required=True)
p.add_argument('--amitcp-data', type=Path, required=True)
p.add_argument('--driver', type=Path, required=True)
p.add_argument('--out', type=Path, required=True)
p.add_argument('--machine', default='A4000'); p.add_argument('--cpu', default='68020')
p.add_argument('--fast-memory', type=int, default=8192)
a = p.parse_args()
for x in (a.workbench, a.rom, a.amitcp_data, a.driver):
    if not x.exists(): p.error(f'missing input: {x}')
if a.out.exists(): p.error(f'output already exists: {a.out}')
a.out.mkdir(parents=True)
system = a.out/'system'; qual = a.out/'qualification'; logs = a.out/'logs'; output = qual/'output'
shutil.copytree(a.workbench, system)
shutil.copytree(a.amitcp_data, system, dirs_exist_ok=True)
(system/'Devs/Networks').mkdir(parents=True, exist_ok=True)
shutil.copyfile(a.driver, system/'Devs/Networks/a2065.device')
(system/'Devs/NetInterfaces').mkdir(parents=True, exist_ok=True)
(system/'Devs/NetInterfaces/eth0').write_text('device=a2065.device\nunit=0\nconfigure=dhcp\nrequiresinitdelay=no\n')
(system/'S').mkdir(exist_ok=True)
(system/'S/Network-Startup').write_text('FailAt 30\nAddNetInterface DEVS:NetInterfaces/eth0 >DH0:output/addnet.txt\nShowNetStatus >DH0:output/netstatus.txt\n')
(qual/'S').mkdir(parents=True); output.mkdir(); logs.mkdir()
(qual/'S/Startup-Sequence').write_text('''C:Echo BOOT_START >DH0:output/00_BOOT_START
DH1:C/Assign SYS: DH1:
DH1:C/Assign C: DH1:C
DH1:C/Assign LIBS: DH1:Libs
DH1:C/Assign DEVS: DH1:Devs
DH1:C/Assign L: DH1:L
DH1:C/Assign FONTS: DH1:Fonts
DH1:C/Assign S: DH1:S
DH1:C/Assign T: RAM:
C:Echo LIBS_READY >DH0:output/10_LIBS_READY
DH1:C/Assign AmiTCP: DH1:Programs/AmiTCP
C:Echo BEFORE_NET >DH0:output/20_BEFORE_NET
DH1:C/AddNetInterface DH1:Devs/NetInterfaces/eth0 >DH0:output/addnet.txt
DH1:C/ShowNetStatus >DH0:output/netstatus.txt
C:Echo DONE >DH0:output/99_DONE
''')
(a.out/'driver-sha256.txt').write_text(hashlib.sha256(a.driver.read_bytes()).hexdigest()+'  a2065.device\n')
(a.out/'amitcp-data-source.txt').write_text(str(a.amitcp_data.resolve())+'\n')
(a.out/'qualification.fs-uae').write_text(f'''[fs-uae]
amiga_model = {a.machine}
cpu = {a.cpu}
chip_memory = 2048
fast_memory = {a.fast_memory}
fpu = 0
bsdsocket_library = 0
network_card = a2065
a2065 = slirp
kickstart_file = {a.rom.resolve()}
hard_drive_0 = {qual.resolve()}
hard_drive_0_label = Qualification
hard_drive_0_priority = 10
hard_drive_1 = {system.resolve()}
hard_drive_1_label = System
hard_drive_1_priority = 0
base_dir = {(a.out/'fs-uae').resolve()}
logs_dir = {logs.resolve()}
fullscreen = 0
sound_output = 0
automatic_input_grab = 0
''')
print(a.out/'qualification.fs-uae')
