#!/bin/sh

distdir="$1"
exportdir="$HOME"/sites/www.vx68k.org/libvm68k/1/src/"$2"

test -d "$distdir" || exit 1

rm -fr "$exportdir"  || exit 1
cp -a "$distdir" "$exportdir" || exit 1

exit 0
