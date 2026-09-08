#!/bin/sh
set -eu

bin=${1:?AmiNTP host binary required}

version=$($bin VERSION)
[ "$version" = "AmiNTP 0.1.0-m0" ]

$bin HELP | grep -q "minimal SNTP client"

set +e
out=$($bin SERVER=pool.ntp.org 2>&1)
rc=$?
set -e

[ "$rc" -eq 5 ]
printf '%s\n' "$out" | grep -q "specify QUERY or SYNC"

echo "M0 host smoke: PASS"
