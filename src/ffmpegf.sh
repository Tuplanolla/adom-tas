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
echo "Encoding filtered..."
sleep 1
i=$(xwininfo -id "$WINDOWID")
w=$(echo "$i" | grep "Width" | cut -d ":" -f 2 | tr -d " ")
h=$(echo "$i" | grep "Height" | cut -d ":" -f 2 | tr -d " ")
x=$(echo "$i" | grep "Absolute[^:]\+X" | cut -d ":" -f 2 | tr -d " ")
y=$(echo "$i" | grep "Absolute[^:]\+Y" | cut -d ":" -f 2 | tr -d " ")
r="16"
q="5"
#pad up
#f="pad=iw+(16-mod(iw\,16)):ih+(16-mod(ih\,16)):(16-mod(iw\,16))/2:(16-mod(ih\,16))/2:black"
#crop down
#f="crop=iw-mod(iw\,16):ih-mod(ih\,16):mod(iw\,16)/2:mod(ih\,16)/2:black"
#scale up
#f="scale=iw+(16-mod(iw\,16)):ih+(16-mod(ih\,16))"
#scale down
f="scale=iw-mod(iw\,16):ih-mod(ih\,16)"
ffmpeg -y -f x11grab -r "$r" -s "$w"x"$h" -i "$DISPLAY"+"$x","$y" -qscale "$q" -vf "$f" "$2" &> /dev/null &
eval "$1"
if test -n "$!"
then kill -QUIT "$!"
fi
