#!/bin/bash
# make_installer.sh: builds the New Adversaries installer (NSIS).
#
#   make_installer.sh VERSION GAMEPLAY_EXE CLASSIC_EXE OFFICIAL_DIR OUT_DIR [MAKENSIS]
#
# GAMEPLAY_EXE  7k-newAdversaries.exe, built from the 7kmod-gameplay branch
# CLASSIC_EXE   7k-newAdversaries-Classic.exe, built from the 7kmod branch
# OFFICIAL_DIR  an installation of the official 7kaa 2.15.7, for the files that
#               aren't in this repository: MUSIC, locale, SDL2.dll, OpenAL32.dll,
#               libcurl-4.dll, 7kaa-manual.pdf, COPYING.txt, COPYING-Music.txt
#               and README-Music.txt
# OUT_DIR       where NewAdversaries-VERSION-setup.exe is written
#
# The game data (ENCYC, IMAGE, SPRITE...), the mod sounds and the documents
# come from this repository. Run it from the 7kmod-gameplay branch, which has
# all the player documents and changelogs.

set -e
VERSION=$1; GAMEPLAY_EXE=$2; CLASSIC_EXE=$3; OFFICIAL=$4; OUT=$5; MAKENSIS=${6:-makensis}
[ -n "$OUT" ] || { sed -n 2,18p "$0"; exit 1; }

HERE=$(cd "$(dirname "$0")" && pwd)
REPO=$(cd "$HERE/../../.." && pwd)
STAGE=$(mktemp -d)
trap 'rm -rf "$STAGE"' EXIT

# executables and the files of the official game
cp "$GAMEPLAY_EXE" "$STAGE/7k-newAdversaries.exe"
cp "$CLASSIC_EXE"  "$STAGE/7k-newAdversaries-Classic.exe"
for f in SDL2.dll OpenAL32.dll libcurl-4.dll 7kaa-manual.pdf; do cp "$OFFICIAL/$f" "$STAGE/"; done
cp -r "$OFFICIAL/MUSIC" "$OFFICIAL/locale" "$STAGE/"

# game data and mod sounds
for d in ENCYC ENCYC2 IMAGE RESOURCE SCENARI2 SCENARIO SOUND SPRITE TUTORIAL; do
	cp -r "$REPO/data/$d" "$STAGE/"
done
find "$STAGE" -name 'Makefile*' -delete
cp -r "$REPO/7kmod/MODSOUND" "$STAGE/"

# read me, licenses, changelogs and player documents (Markdown as .txt, CRLF)
txt() { sed 's/$/\r/' "$1" > "$2"; }
txt "$HERE/LEEME.txt"  "$STAGE/LEEME.txt"
txt "$HERE/README.txt" "$STAGE/README.txt"
mkdir -p "$STAGE/Licencias" "$STAGE/Changelog" "$STAGE/Docs"
for f in COPYING.txt COPYING-Music.txt README-Music.txt; do cp "$OFFICIAL/$f" "$STAGE/Licencias/"; done
for f in CHANGELOG CHANGELOG-gameplay CHANGELOG-fryhtan; do
	txt "$REPO/7kmod/$f.md"    "$STAGE/Changelog/$f.txt"
	txt "$REPO/7kmod/$f.en.md" "$STAGE/Changelog/$f.en.txt"
done
for f in GUIDE KEYBINDINGS GREATER_BEINGS SPYING SIEGE_WEAPONS COMBAT_ANALYSIS ADVANCED_CONFIG; do
	txt "$REPO/7kmod/$f.md"    "$STAGE/Docs/$f.txt"
	txt "$REPO/7kmod/$f.en.md" "$STAGE/Docs/$f.en.txt"
done
cp "$REPO/src/7k.ico" "$STAGE/installer.ico"

mkdir -p "$OUT"
"$MAKENSIS" -V2 -DVERSION="$VERSION" -DSTAGE="$STAGE" \
	-DOUTFILE="$(cd "$OUT" && pwd)/NewAdversaries-$VERSION-setup.exe" "$HERE/newadversaries.nsi"
