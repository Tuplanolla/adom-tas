#! /bin/sh

#Lists the error codes.
NO_PROBLEM="0"
USAGE_PROBLEM="1"
OPT_PROBLEM="2"
COMMAND_PROBLEM="3"
INTERNAL_PROBLEM="4"

#Parses the command-line options.
COMMAND=""
OUTPUT="output.log"
while getopts "he:o:" "OPT"
do
	case "$OPT" in
		"h")
			echo "ltrace.sh [-h] -e command [-o output]

-h     Prints this help.
-e     Executes a command after starting the tracing.
-o     Determines the output file.
       The default value is \"output.log\"."
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

#Checks the existence of the tracer.
LTRACE=`command -v "ltrace"`
if test -z "$LTRACE"
then
	echo "Finding \"ltrace\" failed."
	exit "$COMMAND_PROBLEM"
fi

#Checks the existence of a compiler.
GCC=`command -v "gcc"`
if test -z "$GCC"
then
	echo "Finding \"gcc\" failed."
	exit "$COMMAND_PROBLEM"
fi

#Checks the existence of a linker.
LD=`command -v "ld"`
if test -z "$LD"
then
	echo "Finding \"ld\" failed."
	exit "$COMMAND_PROBLEM"
fi

#Compiles the library to preload.
LD_PATH="/tmp/time"
echo "#include <time.h>
time_t time(time_t * t) {
	if (t != NULL) *t = 0;
	return 0;
}" >"$LD_PATH.c"
"$GCC" -c "$LD_PATH.c" -o "$LD_PATH.o"
rm -f "$LD_PATH.c"
"$LD" -shared "$LD_PATH.o" -o "$LD_PATH.so"
rm -f "$LD_PATH.o"

#Checks the existence of the library to preload.
if test ! -f "$LD_PATH.so"
then
	echo "Compiling and linking the library to preload failed."
	exit "$INTERNAL_PROBLEM"
fi

#Starts the tracing.
echo "Starting \"ltrace\"..."
LD_PRELOAD="$LD_PATH.so" ltrace -s "65535" -o "$OUTPUT" "$COMMAND"
rm -f "$LD_PATH.so"
echo "...done!"
exit "$NO_PROBLEM"
