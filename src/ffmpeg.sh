#!/bin/sh
#ffmpeg.sh -e bin/adom-tas -o output.avi
#ffmpeg.sh -e bin/adom-tas -r 32 -m 16 -s sd -o output.avi
usage() {
	echo "ffmpeg.sh [-h] [-e command] [-r rate] [-m multiple] [-s method] [-o output]

-h     Prints this help.
-e     Executes a command after starting the recording.
-r     Sets the frame rate.
       The minimum value is 1, the maximum 256 and the default 16.
-q     Adjusts the quality.
       The minimum value is 1, the maximum 13 and the default 5.
-m     Changes the number the frame size is a multiple of.
       The minimum value is 2, the maximum 16 and the default 2.
-s     Selects the method to use to change the frame size.
       The default value is cd.
   cu  Rounds up and crops the captured region.
       Expands down and right with the upper left corner fixed.
   cd  Rounds down and crops the captured region.
       Contracts up and left with the upper left corner fixed.
   pu  Rounds up and pads the intermediate region with black borders.
       Expands equally in all directions with the center fixed.
       Requires libavfilter to be supported.
   pd  Rounds down and crops the intermediate region.
       Contracts equally in all directions with the center fixed.
       Requires libavfilter to be supported.
   su  Rounds up and scales the intermediate region.
       Expands equally in all directions with the center fixed.
       Requires libavfilter to be supported.
   sd  Rounds down and scales the intermediate region.
       Contracts equally in all directions with the center fixed.
       Requires libavfilter to be supported.
-o     Determines the output file.
       The default value is \"output.avi\"."
}
XWININFO=$(command -v "xwininfo")
if [ -z $XWININFO ]
then
	echo "Finding \"xwininfo\" failed."
	exit 4
fi
WININFO=$(xwininfo -id "$WINDOWID")
WIDTH=$(echo "$WININFO" | grep "Width" | cut -d ":" -f 2 | tr -d " ")
HEIGHT=$(echo "$WININFO" | grep "Height" | cut -d ":" -f 2 | tr -d " ")
X=$(echo "$WININFO" | grep "Absolute[^:]\+X" | cut -d ":" -f 2 | tr -d " ")
Y=$(echo "$WININFO" | grep "Absolute[^:]\+Y" | cut -d ":" -f 2 | tr -d " ")
FFMPEG=$(command -v "ffmpeg")
if [ -z $FFMPEG ]
then
	echo "Finding \"ffmpeg\" failed."
	exit 4
fi
COMMAND=""
RATE="16"
QUALITY="5"
MULTIPLE="2"
SIZE="cd"
OUTPUT="output.avi"
while getopts "he:r:q:m:f:o:" OPT
do
	case $OPT in
		h)
			usage
			exit 1
			;;
		e)
			COMMAND="$OPTARG"
			;;
		r)
			RATE="$OPTARG"
			;;
		q)
			QUALITY="$OPTARG"
			;;
		m)
			MULTIPLE="$OPTARG"
			;;
		f)
			SIZE="$OPTARG"
			;;
		o)
			OUTPUT="$OPTARG"
			;;
		*)
			echo "Parsing the option \"$OPT\" failed."
			exit 2
	esac
done
FILTERS=$(ffmpeg -filters 2> "/dev/null")
FILTER=""
case $SIZE in
	cu)
		WIDTH=$(expr "$WIDTH" "+" "(" "$MULTIPLE" "-" "$WIDTH" "%" "$MULTIPLE" ")")
		HEIGHT=$(expr "$HEIGHT" "+" "(" "$MULTIPLE" "-" "$HEIGHT" "%" "$MULTIPLE" ")")
		;;
	cd)
		WIDTH=$(expr "$WIDTH" "-" "$WIDTH" "%" "$MULTIPLE")
		HEIGHT=$(expr "$HEIGHT" "-" "$HEIGHT" "%" "$MULTIPLE")
		;;
	pu)
		PAD=$(echo "$FILTERS" | grep "^pad\s")
		if [ -z $PAD ]
		then
			echo "Finding the pad filter failed."
			exit 3
		fi
		FILTER="pad=iw+($MULTIPLE-mod(iw\,$MULTIPLE)):ih+($MULTIPLE-mod(ih\,$MULTIPLE)):(16-mod(iw\,$MULTIPLE))/2:($MULTIPLE-mod(ih\,$MULTIPLE))/2:black"
		;;
	pd)
		CROP=$(echo "$FILTERS" | grep "^crop\s")
		if [ -z $CROP ]
		then
			echo "Finding the crop filter failed."
			exit 3
		fi
		FILTER="crop=iw-mod(iw\,$MULTIPLE):ih-mod(ih\,$MULTIPLE):mod(iw\,$MULTIPLE)/2:mod(ih\,$MULTIPLE)/2:black"
		;;
	su)
		SCALE=$(echo "$FILTERS" | grep "^scale\s")
		if [ -z $SCALE ]
		then
			echo "Finding the scale filter failed."
			exit 3
		fi
		FILTER="scale=iw+($MULTIPLE-mod(iw\,$MULTIPLE)):ih+(16-mod(ih\,$MULTIPLE))"
		;;
	sd)
		SCALE=$(echo "$FILTERS" | grep "^scale\s")
		if [ -z $SCALE ]
		then
			echo "Finding the scale filter failed."
			exit 3
		fi
		FILTER="scale=iw-mod(iw\,$MULTIPLE):ih-mod(ih\,$MULTIPLE)"
		;;
	*)
		echo "Parsing the option \"$SIZE\" failed."
		exit 2
esac
echo "Starting \"ffmpeg\"..."
if [ -z "$FILTER" ]
then
	ffmpeg -y -f x11grab -an -r "$RATE" -s "$WIDTH"x"$HEIGHT" -i "$DISPLAY"+"$X","$Y" -q:v "$QUALITY" "$OUTPUT" &> "/dev/null" &
else
	ffmpeg -y -f x11grab -an -r "$RATE" -s "$WIDTH"x"$HEIGHT" -i "$DISPLAY"+"$X","$Y" -q:v "$QUALITY" -vf "$FILTER" "$OUTPUT" &> "/dev/null" &
fi
FFMPEG_PID="$!"
echo "Starting \"$COMMAND\"..."
eval "$COMMAND" &
COMMAND_PID="$!"
while true
do
	if ! kill -0 "$FFMPEG_PID" &> "/dev/null"
	then
		echo "Stopping since \"ffmpeg\" exited..."
		break
	fi
	if ! kill -0 "$COMMAND_PID" &> "/dev/null"
	then
		echo "Stopping since \"$COMMAND\" exited..."
		break
	fi
	sleep 1
done
if kill -0 "$FFMPEG_PID" &> "/dev/null"
then
	kill -15 "$FFMPEG_PID"
fi
if kill -0 "$COMMAND_PID" &> "/dev/null"
then
	kill -9 "$COMMAND_PID"
fi
