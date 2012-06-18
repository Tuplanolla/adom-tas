#! /bin/sh
echo "Cleaning..."
sleep 1
rm -f "~/.adom.data/.HISCORE"
rm -f "~/.adom.data/.adom.prc"
rm -f -r "~/.adom.data/tmpdat/*"
DIRECTORY=`dirname "$0"`
cp "$DIRECTORY/adom.cnt" "~/.adom.data/.adom.cnt"
cp "$DIRECTORY/adom.cfg" "~/.adom.data/.adom.cfg"
cp "$DIRECTORY/adom.kbd" "~/.adom.data/.adom.kbd"
