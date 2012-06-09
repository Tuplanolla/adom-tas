#!/bin/sh
echo "Cleaning..."
sleep 1
rm -f "~/.adom.data/.HISCORE"
rm -f "~/.adom.data/.adom.prc"
rm -f -r "~/.adom.data/tmpdat/*"
d=$(dirname "$0")
cp "$d""/adom.cnt" "~/.adom.data/.adom.cnt"
cp "$d""/adom.cfg" "~/.adom.data/.adom.cfg"
cp "$d""/adom.kbd" "~/.adom.data/.adom.kbd"
