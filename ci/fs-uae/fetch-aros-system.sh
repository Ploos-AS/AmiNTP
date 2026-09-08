#!/usr/bin/env bash
set -euo pipefail

AROS_INDEX_URL="https://aros.sourceforge.io/cgi-bin/files?lang=en&type=nightly2"
AROS_TARGET="amiga-m68k-boot-iso"
OUT_DIR="${1:-build/fs-uae/aros-system}"

mkdir -p "$OUT_DIR"
index_html="$OUT_DIR/aros-nightly-index.html"

curl --fail --location --retry 3 --retry-delay 2 \
  "$AROS_INDEX_URL" -o "$index_html"

AROS_URL="$(
  { grep -oE 'href="[^"]*amiga-m68k-boot-iso[^"]*"' "$index_html" || true; } \
    | head -n 1 \
    | sed -e 's/^href="//' -e 's/"$//' -e 's/&amp;/\&/g'
)"

if [[ -z "$AROS_URL" ]]; then
  echo "ERROR: could not resolve $AROS_TARGET from $AROS_INDEX_URL" >&2
  exit 1
fi

case "$AROS_URL" in
  http://*|https://*) ;;
  //*) AROS_URL="https:${AROS_URL}" ;;
  /*) AROS_URL="https://aros.sourceforge.io${AROS_URL}" ;;
  *) AROS_URL="https://aros.sourceforge.io/${AROS_URL}" ;;
esac

url_path="${AROS_URL%%\?*}"
if [[ "$url_path" == */download ]]; then
  AROS_ARCHIVE="$(basename "$(dirname "$url_path")")"
else
  AROS_ARCHIVE="$(basename "$url_path")"
fi

if [[ -z "$AROS_ARCHIVE" || "$AROS_ARCHIVE" != *"$AROS_TARGET"* ]]; then
  echo "ERROR: resolved URL does not identify expected AROS target: $AROS_URL" >&2
  exit 1
fi

archive="$OUT_DIR/$AROS_ARCHIVE"
curl --fail --location --retry 3 --retry-delay 2 "$AROS_URL" -o "$archive"
sha256sum "$archive" | tee "$OUT_DIR/archive.sha256"

rm -rf "$OUT_DIR/archive-extracted"
mkdir -p "$OUT_DIR/archive-extracted"
case "$AROS_ARCHIVE" in
  *.lha|*.LHA)
    lha xw="$OUT_DIR/archive-extracted" "$archive" >/dev/null
    ;;
  *.zip|*.ZIP)
    unzip -q "$archive" -d "$OUT_DIR/archive-extracted"
    ;;
  *)
    echo "ERROR: unsupported AROS system archive format: $AROS_ARCHIVE" >&2
    exit 1
    ;;
esac

iso="$(find "$OUT_DIR/archive-extracted" -type f \( -iname '*.iso' -o -iname '*.ISO' \) -print -quit)"
if [[ -z "$iso" ]]; then
  echo "ERROR: no ISO found in $AROS_ARCHIVE" >&2
  exit 1
fi

cp "$iso" "$OUT_DIR/system.iso"
printf 'AROS_INDEX_URL=%s\nAROS_TARGET=%s\nAROS_ARCHIVE=%s\nAROS_URL=%s\nISO=%s\n' \
  "$AROS_INDEX_URL" "$AROS_TARGET" "$AROS_ARCHIVE" "$AROS_URL" "$OUT_DIR/system.iso" \
  > "$OUT_DIR/source.txt"

echo "$OUT_DIR/system.iso"
