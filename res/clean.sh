#! /bin/sh
echo "Cleaning..."
sleep 1
rm -f "~/.adom.data/.HISCORE"
rm -f "~/.adom.data/.adom.prc"
rm -f -r "~/.adom.data/tmpdat/*"
DIRECTORY=`dirname "$0"`
DATA="$HOME/.adom.data"
cp "$DIRECTORY/clean.cnt" "$DATA/.adom.cnt"
cp "$DIRECTORY/clean.kbd" "$DATA/.adom.kbd"
cp "$DIRECTORY/clean.cfg" "$DATA/.adom.cfg"
