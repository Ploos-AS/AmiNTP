#!/usr/bin/env bash
set -euo pipefail

AROS_DATE=20260907
AROS_ARCHIVE="AROS-${AROS_DATE}-amiga-m68k-boot-floppy.lha"
AROS_URL="https://sourceforge.net/projects/aros/files/nightly2/${AROS_DATE}/Binaries/${AROS_ARCHIVE}/download"
OUT_DIR="${1:-build/fs-uae/aros-boot}"

mkdir -p "$OUT_DIR"
archive="$OUT_DIR/$AROS_ARCHIVE"

curl --fail --location --retry 3 --retry-delay 2 "$AROS_URL" -o "$archive"
sha256sum "$archive" | tee "$OUT_DIR/archive.sha256"

rm -rf "$OUT_DIR/extracted"
mkdir -p "$OUT_DIR/extracted"
lha xw="$OUT_DIR/extracted" "$archive" >/dev/null

adf="$(find "$OUT_DIR/extracted" -type f \( -iname '*.adf' -o -iname '*.ADF' \) | head -n 1)"
if [[ -z "$adf" ]]; then
  echo "ERROR: no ADF found in $AROS_ARCHIVE" >&2
  exit 1
fi

cp "$adf" "$OUT_DIR/bootdisk.adf"
printf 'AROS_DATE=%s\nAROS_ARCHIVE=%s\nAROS_URL=%s\nADF=%s\n' \
  "$AROS_DATE" "$AROS_ARCHIVE" "$AROS_URL" "$OUT_DIR/bootdisk.adf" \
  > "$OUT_DIR/source.txt"

echo "$OUT_DIR/bootdisk.adf"
