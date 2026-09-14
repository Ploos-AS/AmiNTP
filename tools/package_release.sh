#!/usr/bin/env bash
set -euo pipefail

VERSION="${1:?usage: tools/package_release.sh <version-without-v>}"
DIST="dist"
ROOT="AmiNTP-v${VERSION}"
STAGE="${DIST}/${ROOT}"
AMINET_ROOT="AmiNTP"
AMINET_STAGE="${DIST}/${AMINET_ROOT}"
AMINTP_BINARY="${AMINTP_BINARY:-AmiNTP}"

if [[ ! -f "$AMINTP_BINARY" ]]; then
  echo "ERROR: native AmiNTP binary not found: $AMINTP_BINARY" >&2
  exit 1
fi

rm -rf "$STAGE" "$AMINET_STAGE"
mkdir -p "$STAGE/docs" "$AMINET_STAGE/docs"

cp "$AMINTP_BINARY" "$STAGE/AmiNTP"
cp README.md LICENSE "$STAGE/"
if [[ -f examples/AmiNTP.conf ]]; then
  mkdir -p "$STAGE/examples"
  cp examples/AmiNTP.conf "$STAGE/examples/"
fi

# Include the integration qualification summaries when present.
for doc in docs/M4_1_QUALIFICATION.md docs/M4_2A_BSDSOCKET_QUALIFICATION.md docs/M4_2B_AMITCP_QUALIFICATION.md; do
  [[ -f "$doc" ]] && cp "$doc" "$STAGE/docs/"
done
for doc in docs/*MIAMI*QUALIFICATION*.md docs/*Miami*QUALIFICATION*.md; do
  [[ -f "$doc" ]] && cp "$doc" "$STAGE/docs/"
done
[[ -f "docs/RELEASE_NOTES_v${VERSION}.md" ]] && cp "docs/RELEASE_NOTES_v${VERSION}.md" "$STAGE/docs/"

cat > "$STAGE/RELEASE.txt" <<EOF
AmiNTP v${VERSION}

Copyright: Ploos AS
Uploader: Per Gustav Ousdal <amiga@ousdal.org>
License: MIT
Target: AmigaOS 2.04+, Motorola 68000+, no FPU required

AmiNTP is a small native SNTP client for classic Amiga systems. It provides
DNS/numeric IPv4 NTP queries, system-clock synchronization, optional RTC
updates, configuration-file support and an ARexx control port.

Native qualification has been completed with AmiTCP_NG 4.1.5 and Miami 3.2b2
on a 68000 / Kickstart 2.04 / Workbench 2.1 configuration. Roadshow remains a
compatibility target but is not part of this release's qualified matrix.
EOF

cat > "$STAGE/AmiNTP.readme" <<EOF
Short:        Native SNTP client for classic Amiga
Uploader:     Per Gustav Ousdal <amiga@ousdal.org>
Author:       Ploos AS
Type:         comm/net
Version:      ${VERSION}
Architecture: m68k-amigaos
Requires:     AmigaOS 2.04+, Motorola 68000, bsdsocket.library
License:      MIT

AmiNTP is a small native SNTP client for classic Amiga systems. It supports
DNS names and IPv4 addresses, NTP queries, system-clock synchronization,
optional RTC updates, configuration files and ARexx control through the
AMINTP port.

The program has no GUI, requires no FPU, and is built for the Motorola 68000.
Native-stack qualification has been completed with AmiTCP_NG 4.1.5 and Miami
3.2b2 on AmigaOS 2.04/Workbench 2.1. Roadshow is a compatibility target but
has not yet been included in the qualified release matrix.

Copyright (c) 2026 Ploos AS
EOF

(
  cd "$STAGE"
  find . -type f -print0 | sort -z | xargs -0 sha256sum > SHA256SUMS
)

rm -f "${DIST}/${ROOT}.zip" "${DIST}/${ROOT}.zip.sha256" \
  "${DIST}/AmiNTP.lha" "${DIST}/AmiNTP.lha.sha256" \
  "${DIST}/AmiNTP.readme"

(
  cd "$DIST"
  zip -qr "${ROOT}.zip" "$ROOT"
)
sha256sum "${DIST}/${ROOT}.zip" > "${DIST}/${ROOT}.zip.sha256"

cp "$STAGE/AmiNTP" "$AMINET_STAGE/AmiNTP"
cp "$STAGE/README.md" "$STAGE/LICENSE" "$STAGE/RELEASE.txt" "$AMINET_STAGE/"
[[ -d "$STAGE/examples" ]] && cp -a "$STAGE/examples" "$AMINET_STAGE/"
if compgen -G "$STAGE/docs/*" >/dev/null; then
  cp -a "$STAGE/docs/." "$AMINET_STAGE/docs/"
fi
cp "$STAGE/AmiNTP.readme" "${DIST}/AmiNTP.readme"

LHA_CREATOR="${LHA_CMD:-}"
if [[ -n "$LHA_CREATOR" ]]; then
  if [[ ! -x "$LHA_CREATOR" ]] && ! command -v "$LHA_CREATOR" >/dev/null 2>&1; then
    echo "ERROR: LHA_CMD does not name an executable archive creator: $LHA_CREATOR" >&2
    exit 2
  fi
  if "$LHA_CREATOR" --version 2>&1 | grep -qi 'lhasa'; then
    echo "ERROR: LHA_CMD points to Lhasa, which cannot create archives" >&2
    exit 2
  fi
else
  for candidate in lha lharc; do
    command -v "$candidate" >/dev/null 2>&1 || continue
    if "$candidate" --version 2>&1 | grep -qi 'lhasa'; then
      continue
    fi
    LHA_CREATOR="$candidate"
    break
  done
fi

if [[ -z "$LHA_CREATOR" ]]; then
  echo "ERROR: a real LHA archive writer is required; set LHA_CMD" >&2
  exit 2
fi

(
  cd "$DIST"
  "$LHA_CREATOR" -aq "AmiNTP.lha" "$AMINET_ROOT"
)
sha256sum "${DIST}/AmiNTP.lha" > "${DIST}/AmiNTP.lha.sha256"
rm -rf "$AMINET_STAGE"

printf 'Release packages created:\n'
printf '  %s\n' "${DIST}/AmiNTP.lha"
printf '  %s\n' "${DIST}/AmiNTP.readme"
printf '  %s\n' "${DIST}/AmiNTP.lha.sha256"
printf '  %s\n' "${DIST}/${ROOT}.zip"
printf '  %s\n' "${DIST}/${ROOT}.zip.sha256"
