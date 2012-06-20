#! /bin/sh

#Sets the location of logs.
NOWHERE="/dev/null"

#Lists the error codes.
NO_PROBLEM="0"
USAGE_PROBLEM="1"
OPT_PROBLEM="2"
COMMAND_PROBLEM="3"
LIBRARY_PROBLEM="4"

#Parses the command-line options.
COMMAND=""
RATE="16"
QUALITY="5"
MULTIPLE="2"
SIZE="cd"
OUTPUT="output.avi"
while getopts "he:r:q:m:f:o:" "OPT"
do
	case "$OPT" in
		"h")
			echo "ffmpeg.sh [-h] -e command [-r rate] [-m multiple] [-s method] [-o output]

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
       Requires libavfilter support.
   pd  Rounds down and crops the intermediate region.
       Contracts equally in all directions with the center fixed.
       Requires libavfilter support.
   su  Rounds up and scales the intermediate region.
       Expands equally in all directions with the center fixed.
       Requires libavfilter support.
   sd  Rounds down and scales the intermediate region.
       Contracts equally in all directions with the center fixed.
       Requires libavfilter support.
-o     Determines the output file.
       The default value is \"output.avi\"."
			exit "$USAGE_PROBLEM"
			;;
		"e")
			COMMAND="$OPTARG"
			;;
		"r")
			RATE="$OPTARG"
			;;
		"q")
			QUALITY="$OPTARG"
			;;
		"m")
			MULTIPLE="$OPTARG"
			;;
		"f")
			SIZE="$OPTARG"
			;;
		"o")
			OUTPUT="$OPTARG"
			;;
		*)
			echo "Parsing the option \"$OPT\" failed."
			exit "$OPT_PROBLEM"
	esac
done

#Finds the dimensions of the window.
XWININFO=`command -v "xwininfo"`
if test -z "$XWININFO"
then
	echo "Finding \"xwininfo\" failed."
	exit "$COMMAND_PROBLEM"
fi
WININFO=`"$XWININFO" -id "$WINDOWID"`
WIDTH=`echo "$WININFO" | grep "Width" | cut -d ":" -f "2" | tr -d " "`
HEIGHT=`echo "$WININFO" | grep "Height" | cut -d ":" -f "2" | tr -d " "`
X=`echo "$WININFO" | grep "Absolute[^:]\+X" | cut -d ":" -f "2" | tr -d " "`
Y=`echo "$WININFO" | grep "Absolute[^:]\+Y" | cut -d ":" -f "2" | tr -d " "`

#Checks the existence of the recorder.
FFMPEG=`command -v "ffmpeg"`
if test -z "$FFMPEG"
then
	echo "Finding \"ffmpeg\" failed."
	exit "$COMMAND_PROBLEM"
fi

#Starts the recording.
FILTERS=`"$FFMPEG" -filters 2>"$NOWHERE"`
FILTER=""
case "$SIZE" in
	"cu")
		WIDTH=`expr "$WIDTH" "+" "(" "$MULTIPLE" "-" "$WIDTH" "%" "$MULTIPLE" ")"`
		HEIGHT=`expr "$HEIGHT" "+" "(" "$MULTIPLE" "-" "$HEIGHT" "%" "$MULTIPLE" ")"`
		;;
	"cd")
		WIDTH=`expr "$WIDTH" "-" "$WIDTH" "%" "$MULTIPLE"`
		HEIGHT=`expr "$HEIGHT" "-" "$HEIGHT" "%" "$MULTIPLE"`
		;;
	"pu")
		PAD=`echo "$FILTERS" | grep "^pad\s"`
		if test -z "$PAD"
		then
			echo "Finding the pad filter failed."
			exit "$LIBRARY_PROBLEM"
		fi
		FILTER="pad=iw+($MULTIPLE-mod(iw\,$MULTIPLE)):ih+($MULTIPLE-mod(ih\,$MULTIPLE)):(16-mod(iw\,$MULTIPLE))/2:($MULTIPLE-mod(ih\,$MULTIPLE))/2:black"
		;;
	"pd")
		CROP=`echo "$FILTERS" | grep "^crop\s"`
		if test -z "$CROP"
		then
			echo "Finding the crop filter failed."
			exit "$LIBRARY_PROBLEM"
		fi
		FILTER="crop=iw-mod(iw\,$MULTIPLE):ih-mod(ih\,$MULTIPLE):mod(iw\,$MULTIPLE)/2:mod(ih\,$MULTIPLE)/2:black"
		;;
	"su")
		SCALE=`echo "$FILTERS" | grep "^scale\s"`
		if test -z "$SCALE"
		then
			echo "Finding the scale filter failed."
			exit "$LIBRARY_PROBLEM"
		fi
		FILTER="scale=iw+($MULTIPLE-mod(iw\,$MULTIPLE)):ih+(16-mod(ih\,$MULTIPLE))"
		;;
	"sd")
		SCALE=`echo "$FILTERS" | grep "^scale\s"`
		if test -z "$SCALE"
		then
			echo "Finding the scale filter failed."
			exit "$LIBRARY_PROBLEM"
		fi
		FILTER="scale=iw-mod(iw\,$MULTIPLE):ih-mod(ih\,$MULTIPLE)"
		;;
	*)
		echo "Parsing the option \"$SIZE\" failed."
		exit "$OPT_PROBLEM"
esac

#Starts the recording.
echo "Starting \"ffmpeg\"..."
if test -z "$FILTER"
then
	"$FFMPEG" -y -f "x11grab" -an -r "$RATE" -s "$WIDTH"x"$HEIGHT" -i "$DISPLAY"+"$X","$Y" -q:v "$QUALITY" "$OUTPUT" 1>"$NOWHERE" 2>"$NOWHERE" &
else
	"$FFMPEG" -y -f "x11grab" -an -r "$RATE" -s "$WIDTH"x"$HEIGHT" -i "$DISPLAY"+"$X","$Y" -q:v "$QUALITY" -vf "$FILTER" "$OUTPUT" 1>"$NOWHERE" 2>"$NOWHERE" &
fi
FFMPEG_PID="$!"
echo "Starting \"$COMMAND\"..."
eval "$COMMAND" &
COMMAND_PID="$!"

#Monitors the recording.
while true
do
	sleep 1
	if ! kill -0 "$FFMPEG_PID" 1>"$NOWHERE" 2>"$NOWHERE"
	then
		echo "Stopping since \"ffmpeg\" exited..."
		break
	fi
	if ! kill -0 "$COMMAND_PID" 1>"$NOWHERE" 2>"$NOWHERE"
	then
		echo "Stopping since \"$COMMAND\" exited..."
		break
	fi
done

#Stops the recording.
if kill -0 "$FFMPEG_PID" 1>"$NOWHERE" 2>"$NOWHERE"
then
	echo "Terminating \"ffmpeg\"..."
	kill -15 "$FFMPEG_PID"
fi
if kill -0 "$COMMAND_PID" 1>"$NOWHERE" 2>"$NOWHERE"
then
	echo "Killing \"$COMMAND\"..."
	kill -9 "$COMMAND_PID"
fi
echo "...done!"
exit "$NO_PROBLEM"
