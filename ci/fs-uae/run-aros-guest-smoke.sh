#!/usr/bin/env bash
set -euo pipefail

OUT_DIR="${1:-build/fs-uae/aros-guest}"
SYSTEM_DIR="build/fs-uae/aros-system"
mkdir -p "$OUT_DIR"

if [[ ! -f build/fs-uae/native/AmiNTP ]]; then
  echo "ERROR: native AmiNTP binary missing; run Gate 3 first" >&2
  exit 1
fi

iso="$(ci/fs-uae/fetch-aros-system.sh "$SYSTEM_DIR" | tail -n 1)"
root_extract="$OUT_DIR/system-root"
rm -rf "$root_extract"
mkdir -p "$root_extract"
7z x -y -o"$root_extract" "$iso" >/dev/null

startup="$(find "$root_extract" -type f -ipath '*/s/startup-sequence' -print -quit)"
if [[ -z "$startup" ]]; then
  echo "ERROR: AROS system ISO does not contain S/Startup-Sequence" >&2
  find "$root_extract" -maxdepth 3 -type f | sort > "$OUT_DIR/system-files.txt"
  exit 1
fi

aros_root="$(dirname "$(dirname "$startup")")"
cp build/fs-uae/native/AmiNTP "$aros_root/AmiNTP"
cp "$startup" "$startup.amintp-original"

cat > "$startup" <<'EOF'
SYS:C/Echo "M3_3C_GUEST_STARTED=1" >SYS:amintp-m3.3c-started.txt
SYS:AmiNTP VERSION >SYS:amintp-m3.3c-version.txt
SYS:C/Execute SYS:S/Startup-Sequence.amintp-original
EOF

rm -f "$aros_root/amintp-m3.3c-started.txt" "$aros_root/amintp-m3.3c-version.txt"

config="$OUT_DIR/aros-guest.fs-uae"
sed "s|@AROS_ROOT@|$PWD/$aros_root|" ci/fs-uae/aros-guest.fs-uae > "$config"
fs-uae --version > "$OUT_DIR/fs-uae-version.txt" 2>&1 || true

set +e
timeout 45s xvfb-run -a fs-uae "$config" > "$OUT_DIR/fs-uae.log" 2>&1
rc=$?
set -e

started="$aros_root/amintp-m3.3c-started.txt"
version_out="$aros_root/amintp-m3.3c-version.txt"
status=FAIL
observation=guest_result_missing

if [[ -f "$started" && -f "$version_out" ]] && grep -q 'AmiNTP 0.3.2-m3.2' "$version_out"; then
  status=PASS
  observation=guest_executed_native_amintp_version
fi

{
  echo "STATUS=$status"
  echo "GATE=M3_3C_AROS_GUEST_EXECUTION"
  echo "MODEL=A1200"
  echo "KICKSTART=internal"
  echo "AROS_ROOT=$aros_root"
  echo "FS_UAE_EXIT=$rc"
  echo "OBSERVATION=$observation"
  if [[ -f "$version_out" ]]; then
    tr -d '\r' < "$version_out" | sed 's/^/GUEST_VERSION=/'
  fi
} | tee "$OUT_DIR/result.txt"

[[ "$status" == PASS ]]
