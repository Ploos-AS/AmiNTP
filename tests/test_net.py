#!/usr/bin/env python3
"""Host-only adversarial datagrams. Never evidence for AmiTCP qualification."""
import socket
import struct
import subprocess
import sys
import threading
import time

binary = sys.argv[1]
seen = []

def case(mode, expected):
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as server:
        server.bind(('127.0.0.1', 0))
        server.settimeout(5)
        errors = []
        def respond():
            try:
                request, peer = server.recvfrom(2048)
                assert len(request) == 48
                assert request[40:48] != bytes(8)
                seen.append(request[40:48])
                reply = bytearray(48)
                reply[0], reply[1] = 0x24, 2
                reply[24:32] = request[40:48]
                struct.pack_into('!II', reply, 40, int(time.time()) + 2208988800, 1)
                if mode == 'origin': reply[24] ^= 1
                if mode == 'short': reply = reply[:47]
                if mode == 'oversize': reply += bytes(1024)
                if mode in ('port', 'address'):
                    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as other:
                        other.bind(('127.0.0.2' if mode == 'address' else '127.0.0.1',
                                    server.getsockname()[1] if mode == 'address' else 0))
                        other.sendto(reply, peer)
                else:
                    server.sendto(reply, peer)
            except BaseException as exc:
                errors.append(exc)
        t = threading.Thread(target=respond)
        t.start()
        result = subprocess.run([binary, 'QUERY', 'SERVER=127.0.0.1',
                                 f'PORT={server.getsockname()[1]}', 'TIMEOUT=2', 'RETRIES=0'],
                                capture_output=True, text=True, timeout=8)
        t.join()
        if errors: raise errors[0]
        assert result.returncode == expected, (mode, result)
        if expected == 0: assert 'OK QUERY' in result.stdout
        else: assert 'query failed' in result.stderr
        print(f'M4.2 host UDP {mode}: PASS (RC {result.returncode})')

for mode in ['valid', 'address', 'port', 'origin', 'short', 'oversize']:
    case(mode, 0 if mode == 'valid' else 10)
assert len(set(seen)) == len(seen), 'request timestamps repeated'
