#! /usr/bin/env bash
$EXTRACTRC `find . -name \*.ui -o -name \*.rc` >> rc.cpp
$XGETTEXT `find . -name \*.cpp -o -name \*.h` rc.cpp -o $podir/kcm_kopete_scripting.pot
rm -f rc.cpp
