/**
Provides error handling.

Errors are major problems.
Warnings are minor problems.
Notes are not problems.
**/
#ifndef PROBLEM_H
#define PROBLEM_H

/**
Lists the error codes.
**/
enum problem_e {
	NO_PROBLEM,
	CONFIG_FIND_PROBLEM,
	CONFIG_PROBLEM,
	LD_PRELOAD_FIND_PROBLEM,
	LD_PRELOAD_ACCESS_PROBLEM,
	LD_PRELOAD_SET_PROBLEM,
	CONFIG_EXEC_PROBLEM,
	EXEC_FIND_PROBLEM,
	EXEC_ACCESS_PROBLEM,
	EXEC_SIZE_PROBLEM,
	CONFIG_DATA_PROBLEM,
	DATA_FIND_PROBLEM,
	VERSION_FIND_PROBLEM,
	VERSION_PROBLEM,
	PROCESS_PROBLEM,
	EXEC_PROBLEM,
	LIBC_FIND_PROBLEM,
	LIBC_ACCESS_PROBLEM,
	LIBNCURSES_FIND_PROBLEM,
	LIBNCURSES_ACCESS_PROBLEM,
	LIBC_PROBLEM,
	LIBNCURSES_PROBLEM,
	LD_PRELOAD_UNSET_PROBLEM,
	CONFIG_ROW_PROBLEM,
	CONFIG_COL_PROBLEM,
	CONFIG_INPUT_PROBLEM,
	INPUT_FIND_PROBLEM,
	INPUT_ACCESS_PROBLEM,
	CONFIG_OUTPUT_PROBLEM,
	OUTPUT_OVERWRITE_PROBLEM,
	OUTPUT_ACCESS_PROBLEM,
	CONFIG_ERROR_LOG_PROBLEM,
	ERROR_LOG_OVERWRITE_PROBLEM,
	ERROR_LOG_ACCESS_PROBLEM,
	CONFIG_WARNING_LOG_PROBLEM,
	WARNING_LOG_OVERWRITE_PROBLEM,
	WARNING_LOG_ACCESS_PROBLEM,
	CONFIG_NOTE_LOG_PROBLEM,
	NOTE_LOG_OVERWRITE_PROBLEM,
	NOTE_LOG_ACCESS_PROBLEM,
	CONFIG_CALL_LOG_PROBLEM,
	CALL_LOG_OVERWRITE_PROBLEM,
	CALL_LOG_ACCESS_PROBLEM,
	CALL_LOG_PROBLEM,
	LOG_CHANGE_PROBLEM
};
typedef enum problem_e problem_t;

/**
Converts an error code to an error message.

@param error The error code.
@return The error message.
**/
inline char * problem_message(const problem_t code) {
	if (code == NO_PROBLEM) return "Nothing failed.";
	if (code == CONFIG_FIND_PROBLEM) return "Finding the configuration file failed. It will be created from a template.";
	if (code == CONFIG_PROBLEM) return "Parsing the configuration file failed.";
	if (code == LD_PRELOAD_FIND_PROBLEM) return "Finding the library to preload \"ld_preload\" in the configuration file or the environment variable \"LD_PRELOAD\" failed.";
	if (code == LD_PRELOAD_ACCESS_PROBLEM) return "Accessing the library to preload failed.";
	if (code == LD_PRELOAD_SET_PROBLEM) return "Setting the library to preload failed.";
	if (code == CONFIG_EXEC_PROBLEM) return "Finding the location of the executable \"exec\" in the configuration file failed.";
	if (code == EXEC_FIND_PROBLEM) return "Finding the executable failed.";
	if (code == EXEC_ACCESS_PROBLEM) return "Accessing the executable failed.";
	if (code == EXEC_SIZE_PROBLEM) return "Verifying the size of the executable failed.";
	if (code == CONFIG_DATA_PROBLEM) return "Finding the location of the executable's data \"data\" in the configuration file failed.";
	if (code == DATA_FIND_PROBLEM) return "Finding the executable's data failed.";
	if (code == VERSION_FIND_PROBLEM) return "Finding the version of the executable failed.";
	if (code == VERSION_PROBLEM) return "Verifying the version of the executable failed.";
	if (code == PROCESS_PROBLEM) return "Removing the process file of the executable failed.";
	if (code == EXEC_PROBLEM) return "Launching the executable failed.";
	if (code == LIBC_FIND_PROBLEM) return "Finding the C standard library \"libc\" in the configuration file failed.";
	if (code == LIBC_ACCESS_PROBLEM) return "Accessing the C standard library failed.";
	if (code == LIBNCURSES_FIND_PROBLEM) return "Finding the New Cursor Optimization library \"libncurses\" in the configuration file failed.";
	if (code == LIBNCURSES_ACCESS_PROBLEM) return "Accessing the New Cursor Optimization library failed.";
	if (code == LIBC_PROBLEM) return "Loading the C standard library failed.";
	if (code == LIBNCURSES_PROBLEM) return "Loading the New Cursor Optimization library failed.";
	if (code == LD_PRELOAD_UNSET_PROBLEM) return "Unsetting the library to preload failed.";
	if (code == CONFIG_ROW_PROBLEM) return "Finding the height of the terminal \"rows\" in the configuration file failed. The default height will be assumed.";
	if (code == CONFIG_COL_PROBLEM) return "Finding the width of the terminal \"cols\" in the configuration file failed. The default width will be assumed.";
	if (code == CONFIG_INPUT_PROBLEM) return "Finding the location of the input file \"input\" in the configuration file failed.";
	if (code == INPUT_FIND_PROBLEM) return "Finding the input file failed.";
	if (code == INPUT_ACCESS_PROBLEM) return "Accessing the input file failed.";
	if (code == CONFIG_OUTPUT_PROBLEM) return "Finding the location of the output file \"output\" in the configuration file failed.";
	if (code == OUTPUT_OVERWRITE_PROBLEM) return "The output file already exists and will be overwritten.";
	if (code == OUTPUT_ACCESS_PROBLEM) return "Accessing the output file failed.";
	if (code == CONFIG_ERROR_LOG_PROBLEM) return "Finding the location of the error log \"errors\" in the configuration file failed. Errors will be logged to the standard error stream \"stderr\" instead.";
	if (code == ERROR_LOG_OVERWRITE_PROBLEM) return "The error log already exists and will be overwritten.";
	if (code == ERROR_LOG_ACCESS_PROBLEM) return "Accessing the error log failed.";
	if (code == CONFIG_WARNING_LOG_PROBLEM) return "Finding the location of the warning log \"warnings\" in the configuration file failed. Warnings will be logged to the standard error stream \"stderr\" instead.";
	if (code == WARNING_LOG_OVERWRITE_PROBLEM) return "The warning log already exists and will be overwritten.";
	if (code == WARNING_LOG_ACCESS_PROBLEM) return "Accessing the warning log failed.";
	if (code == CONFIG_NOTE_LOG_PROBLEM) return "Finding the location of the note log \"notes\" in the configuration file failed. Notes will be logged to the standard error stream \"stderr\" instead.";
	if (code == NOTE_LOG_OVERWRITE_PROBLEM) return "The note log already exists and will be overwritten.";
	if (code == NOTE_LOG_ACCESS_PROBLEM) return "Accessing the note log failed.";
	if (code == CONFIG_CALL_LOG_PROBLEM) return "Finding the location of the call log \"calls\" in the configuration file failed. Calls will not be logged.";
	if (code == CALL_LOG_OVERWRITE_PROBLEM) return "The call log already exists and will be overwritten.";
	if (code == CALL_LOG_ACCESS_PROBLEM) return "Accessing the call log failed.";
	if (code == CALL_LOG_PROBLEM) return "The call log is verbose and requires a lot of resources.";
	if (code == LOG_CHANGE_PROBLEM) return "The log streams have changed and will be redirected.";
	return "Converting an error code to an error message failed.";
}

#endif
