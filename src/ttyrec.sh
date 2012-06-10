#!/bin/sh
echo "Recording..."
sleep 1
ttyrec -e "bin/adom-tas" "output.tty"
