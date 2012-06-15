#!/bin/sh
#ttyrec.sh -e bin/adom-tas -o output.tty
usage() {
	echo "ttyrec.sh [-h] [-e command] [-o output]

-h     Prints this help.
-e     Executes a command after starting the recording.
-o     Determines the output file.
       The default value is \"output.tty\"."
}
TTYREC=$(command -v "ttyrec")
if [ -z $TTYREC ]
then
	echo "Finding \"ttyrec\" failed."
	exit 4
fi
COMMAND=""
OUTPUT="output.tty"
while getopts "he:o:" OPT
do
	case $OPT in
		h)
			usage
			exit 1
			;;
		e)
			COMMAND="$OPTARG"
			;;
		o)
			OUTPUT="$OPTARG"
			;;
		*)
			echo "Parsing the option \"$OPT\" failed."
			exit 2
	esac
done
echo "Starting \"ttyrec\"..."
ttyrec -e "$COMMAND" "$OUTPUT"
echo "Stopping since \"ttyrec\" exited..."
