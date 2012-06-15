#! /bin/sh

#Lists the error codes.
NO_PROBLEM="0"
USAGE_PROBLEM="1"
OPT_PROBLEM="2"
COMMAND_PROBLEM="3"

#Parses the command-line options.
COMMAND=""
OUTPUT="output.tty"
while getopts "he:o:" "OPT"
do
	case "$OPT" in
		"h")
			echo "ttyrec.sh [-h] -e command [-o output]

-h     Prints this help.
-e     Executes a command after starting the recording.
-o     Determines the output file.
       The default value is \"output.tty\"."
			exit "$USAGE_PROBLEM"
			;;
		"e")
			COMMAND="$OPTARG"
			;;
		"o")
			OUTPUT="$OPTARG"
			;;
		*)
			echo "Parsing the option \"$OPT\" failed."
			exit "$OPT_PROBLEM"
	esac
done

#Checks the existence of the recorder.
TTYREC=`command -v "ttyrec"`
if test -z "$TTYREC"
then
	echo "Finding \"ttyrec\" failed."
	exit "$COMMAND_PROBLEM"
fi

#Manages the recording.
echo "Starting \"ttyrec\"..."
"$TTYREC" -e "$COMMAND" "$OUTPUT"
echo "Stopping since \"$COMMAND\" and \"ttyrec\" exited..."
echo "...done!"
exit "$NO_PROBLEM"
