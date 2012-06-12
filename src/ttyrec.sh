#!/bin/sh
if test -z "command -v ttyrec"
then echo "Finding ttyrec failed."
exit
fi
if test -z "$1"
then echo "Finding the command to execute failed."
exit
fi
if test -z "$2"
then echo "Finding the output file failed."
exit
fi
echo "Recording..."
sleep 1
ttyrec -e "$1" "$2"
