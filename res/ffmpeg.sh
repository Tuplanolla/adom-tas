#!/bin/sh
echo "Encoding..."
sleep 1
#simple
#ffmpeg -i "output.tty" "output.avi"
#complicated
i=$(xwininfo -id "$WINDOWID")
s="s/.*"
e=": *\([0-9]\+\).*/\1/"
w=$(echo $i | sed "$s""Width""$e")
h=$(echo $i | sed "$s""Height""$e")
x=$(echo $i | sed "$s""Absolute[^:]\+X""$e")
y=$(echo $i | sed "$s""Absolute[^:]\+Y""$e")
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
ffmpeg -y -f x11grab -r "$r" -s "$w"x"$h" -i "$DISPLAY"+"$x","$y" -qscale "$q" -vf "$f" "output.avi" &> /dev/null &
ttyplay "output.tty"
if test -n "$!"
then kill -QUIT $!
fi
