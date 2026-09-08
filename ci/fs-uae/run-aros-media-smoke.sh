#!/usr/bin/env bash
set -euo pipefail

OUT_DIR=build/fs-uae/aros-media
mkdir -p "$OUT_DIR"

adf="$(ci/fs-uae/fetch-aros-boot.sh build/fs-uae/aros-boot | tail -n 1)"
config="$OUT_DIR/aros-media.fs-uae"
sed "s|@AROS_BOOT_ADF@|$PWD/$adf|" ci/fs-uae/aros-media.fs-uae > "$config"

fs-uae --version > "$OUT_DIR/fs-uae-version.txt" 2>&1 || true

set +e
timeout 25s xvfb-run -a fs-uae "$config" > "$OUT_DIR/fs-uae.log" 2>&1
rc=$?
set -e

{
  if [[ "$rc" -eq 124 ]]; then
    echo "STATUS=PASS"
  else
    echo "STATUS=FAIL"
  fi
  echo "GATE=FS_UAE_AROS_BOOT_MEDIA"
  echo "KICKSTART=internal"
  echo "MODEL=A1200"
  echo "BOOT_MEDIA=$adf"
  echo "FS_UAE_EXIT=$rc"
  if [[ "$rc" -eq 124 ]]; then
    echo "OBSERVATION=emulator_remained_running_for_25_seconds_with_pinned_aros_boot_floppy"
  else
    echo "OBSERVATION=emulator_exited_before_timeout"
  fi
} | tee "$OUT_DIR/result.txt"

[[ "$rc" -eq 124 ]]
