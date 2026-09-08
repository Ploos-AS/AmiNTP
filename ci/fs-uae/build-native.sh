#!/usr/bin/env bash
set -euo pipefail

IMAGE="${AMINTP_BEBBO_IMAGE:-amigadev/m68k-amigaos-gcc:latest}"
OUT_DIR="${1:-build/fs-uae/native}"
mkdir -p "$OUT_DIR"

# Pull explicitly so the exact image digest used by CI is captured as evidence.
docker pull "$IMAGE"
docker image inspect "$IMAGE" --format '{{join .RepoDigests "\n"}}' | tee "$OUT_DIR/toolchain-image.txt"

docker run --rm \
  -v "$PWD:/work" \
  -w /work \
  "$IMAGE" \
  sh -lc 'make clean && make all CC=m68k-amigaos-gcc LDFLAGS=-noixemul'

mkdir -p "$OUT_DIR"
cp AmiNTP "$OUT_DIR/AmiNTP"
file "$OUT_DIR/AmiNTP" | tee "$OUT_DIR/file.txt"
sha256sum "$OUT_DIR/AmiNTP" | tee "$OUT_DIR/AmiNTP.sha256"

if ! grep -Eiq 'AmigaOS|Amiga.*executable|loadseg' "$OUT_DIR/file.txt"; then
  echo "ERROR: native output is not recognized as an Amiga executable" >&2
  exit 1
fi

printf 'STATUS=PASS\nGATE=M3_3B_NATIVE_BEBBO_BUILD\nIMAGE=%s\nBINARY=%s\n' \
  "$IMAGE" "$OUT_DIR/AmiNTP" | tee "$OUT_DIR/result.txt"
