#!/bin/sh
if test -z "command -v xwininfo"
then echo "Finding xwininfo failed."
exit
fi
if test -z "command -v ffmpeg"
then echo "Finding ffmpeg failed."
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
echo "Encoding..."
sleep 1
i=$(xwininfo -id "$WINDOWID")
w=$(echo "$i" | grep "Width" | cut -d ":" -f 2 | tr -d " ")
h=$(echo "$i" | grep "Height" | cut -d ":" -f 2 | tr -d " ")
x=$(echo "$i" | grep "Absolute[^:]\+X" | cut -d ":" -f 2 | tr -d " ")
y=$(echo "$i" | grep "Absolute[^:]\+Y" | cut -d ":" -f 2 | tr -d " ")
r="16"
q="5"
#crop up
wu=$(expr "$w" "+" "(" 16 "-" "$w" "%" 16 ")")
hu=$(expr "$h" "+" "(" 16 "-" "$h" "%" 16 ")")
#crop down
wd=$(expr "$w" "-" "$w" "%" 16)
hd=$(expr "$h" "-" "$h" "%" 16)
ffmpeg -y -f x11grab -r "$r" -s "$wu"x"$hu" -i "$DISPLAY"+"$x","$y" -qscale "$q" "$2" &> /dev/null &
eval "$1"
if test -n "$!"
then kill -QUIT "$!"
fi