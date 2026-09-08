#!/usr/bin/env bash
set -euo pipefail

IMAGE="${AMINTP_BEBBO_IMAGE:-amigadev/m68k-amigaos-gcc:latest}"
OUT_DIR="${1:-build/fs-uae/native}"
mkdir -p "$OUT_DIR"

# Pull explicitly so the exact image digest used by CI is captured as evidence.
docker pull "$IMAGE"
docker image inspect "$IMAGE" --format '{{join .RepoDigests "\n"}}' | tee "$OUT_DIR/toolchain-image.txt"

# The toolchain image intentionally contains the compiler, not a full host build
# environment. Compile and link directly instead of assuming GNU make exists.
docker run --rm \
  -v "$PWD:/work" \
  -w /work \
  "$IMAGE" \
  m68k-amigaos-gcc \
    -Iinclude \
    -Os -Wall -Wextra -Werror -m68000 \
    -noixemul \
    -o AmiNTP \
    src/main.c \
    src/cli.c \
    src/version.c \
    src/sntp.c \
    src/query.c \
    src/time.c \
    src/sync.c \
    src/arexx_core.c \
    src/arexx_ops.c \
    src/net_amiga.c \
    src/clock_amiga.c \
    src/rtc_amiga.c \
    src/arexx_amiga.c

cp AmiNTP "$OUT_DIR/AmiNTP"
file "$OUT_DIR/AmiNTP" | tee "$OUT_DIR/file.txt"
sha256sum "$OUT_DIR/AmiNTP" | tee "$OUT_DIR/AmiNTP.sha256"

if ! grep -Eiq 'AmigaOS|Amiga.*executable|loadseg' "$OUT_DIR/file.txt"; then
  echo "ERROR: native output is not recognized as an Amiga executable" >&2
  exit 1
fi

printf 'STATUS=PASS\nGATE=M3_3B_NATIVE_BEBBO_BUILD\nIMAGE=%s\nBINARY=%s\n' \
  "$IMAGE" "$OUT_DIR/AmiNTP" | tee "$OUT_DIR/result.txt"
