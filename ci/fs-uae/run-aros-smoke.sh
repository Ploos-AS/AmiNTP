#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
RESULT_DIR="${AMINTP_FS_UAE_RESULT_DIR:-$ROOT/build/fs-uae}"
CONFIG="$ROOT/ci/fs-uae/aros-smoke.fs-uae"
LOG="$RESULT_DIR/fs-uae.log"
RESULT="$RESULT_DIR/result.txt"
VERSION="$RESULT_DIR/fs-uae-version.txt"

mkdir -p "$RESULT_DIR"
: >"$LOG"
: >"$RESULT"

fail() {
    {
        echo "STATUS=FAIL"
        echo "GATE=FS_UAE_AROS_BOOT_SMOKE"
        echo "REASON=$1"
    } >"$RESULT"
    cat "$RESULT"
    if [[ -s "$LOG" ]]; then
        echo "--- fs-uae.log ---"
        cat "$LOG"
    fi
    exit 1
}

command -v fs-uae >/dev/null 2>&1 || fail "FS_UAE_NOT_FOUND"
command -v xvfb-run >/dev/null 2>&1 || fail "XVFB_NOT_FOUND"
command -v timeout >/dev/null 2>&1 || fail "TIMEOUT_NOT_FOUND"
[[ -f "$CONFIG" ]] || fail "CONFIG_NOT_FOUND"

fs-uae --version >"$VERSION" 2>&1 || fail "FS_UAE_VERSION_FAILED"

set +e
timeout --signal=TERM 20s xvfb-run -a fs-uae "$CONFIG" >"$LOG" 2>&1
rc=$?
set -e

if [[ $rc -ne 124 ]]; then
    fail "FS_UAE_EXITED_EARLY_RC_${rc}"
fi

{
    echo "STATUS=PASS"
    echo "GATE=FS_UAE_AROS_BOOT_SMOKE"
    echo "KICKSTART=internal"
    echo "MODEL=A1200"
    echo "FS_UAE_EXIT=124"
    echo "OBSERVATION=emulator_remained_running_for_20_seconds"
} >"$RESULT"

cat "$RESULT"
