/**
Handles errors.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef PROBLEM_H
#define PROBLEM_H

/**
Lists the error codes.
**/
enum problem_e {
	NO_PROBLEM
};
typedef enum problem_e problem_t;

/**
Converts an error code to an error message.

@param code The error code.
@return The error message.
**/
/*
CONFIG_OPEN_PROBLEM "Opening the configuration file for writing failed.";
CONFIG_WRITE_PROBLEM "Writing the configuration file failed.";
CONFIG_CLOSE_PROBLEM "Closing the configuration file failed.";
CONFIG_STAT_PROBLEM "Finding the configuration file failed. It will be created from a template.";
CONFIG_PARSE_PROBLEM "Parsing the configuration file failed.";
HOME_CONFIG_PROBLEM "Finding the location of the home directory in the configuration file failed.";
HOME_GETENV_PROBLEM "Finding the location of the home directory in the environment variable failed.";
HOME_GETPWUID_PROBLEM "Finding the location of the home directory in the environment variable failed. It will be guessed.";
HOME_STAT_PROBLEM "Accessing the home directory failed. Shell expansions will be disabled.";
LD_PRELOAD_CONFIG_PROBLEM "Finding the location of the library to preload in the configuration file failed.";
LD_PRELOAD_GETENV_PROBLEM "Finding the location of the library to preload in the environment variable failed. It will be guessed.";
LD_PRELOAD_STAT_PROBLEM "Accessing the library to preload failed.";
LD_PRELOAD_SETENV_PROBLEM "Setting the library to preload in the environment variable failed.";
EXECUTABLE_CONFIG_PROBLEM "Finding the location of the executable in the configuration file failed. It will be guessed.";
EXECUTABLE_STAT_PROBLEM "Accessing the executable failed.";
EXECUTABLE_TYPE_PROBLEM "Ensuring the executable is a file failed.";
EXECUTABLE_PERMISSION_PROBLEM "Checking the permissions of the executable failed.";
EXECUTABLE_SIZE_PROBLEM "Verifying the size of the executable failed.";
EXECUTABLE_OPEN_PROBLEM "Opening the executable for reading failed.";
EXECUTABLE_READ_PROBLEM "Reading the executable failed.";
EXECUTABLE_HASH_PROBLEM "Verifying the hash code of the executable failed.";
EXECUTABLE_CLOSE_PROBLEM "Closing the executable failed.";
EXECUTABLE_DATA_CONFIG_PROBLEM "Finding the location of the executable's data directory in the configuration file failed. It will be guessed.";
EXECUTABLE_DATA_HOME_PROBLEM "Guessing the location of the executable's data directory failed.";
EXECUTABLE_DATA_STAT_PROBLEM "Accessing the executable's data directory in the configuration file failed.";
EXECUTABLE_PROCESS_STAT_PROBLEM "Accessing the executable's process file failed.";
EXECUTABLE_VERSION_STAT_PROBLEM "Accessing the executable's version file failed.";
EXECUTABLE_COUNT_STAT_PROBLEM "Accessing the executable's count file failed.";
EXECUTABLE_KEYBIND_STAT_PROBLEM "Accessing the executable's keybinding file failed.";
EXECUTABLE_CONFIG_STAT_PROBLEM "Accessing the executable's configuration file failed.";

COUNT_OPEN_PROBLEM "Opening the count file for writing failed.";
COUNT_WRITE_PROBLEM "Writing the count file failed.";
COUNT_CLOSE_PROBLEM "Closing the count file failed.";
VERSION_OPEN_PROBLEM
VERSION_READ_PROBLEM
VERSION_CLOSE_PROBLEM
VERSION_MISMATCH_PROBLEM
PROCESS_UNLINK_PROBLEM
EXEC_PROBLEM
*/
inline char * problem_message(const problem_t code) {
	if (code == NO_PROBLEM) return "Nothing failed.";
	if (code == CONFIG_FIND_PROBLEM) return "Finding the configuration file failed. It will be created from a template.";
	if (code == CONFIG_PROBLEM) return "Parsing the configuration file failed.";
	if (code == LD_PRELOAD_FIND_PROBLEM) return "Finding the library to preload \"ld_preload\" in the configuration file or the environment variable \"LD_PRELOAD\" failed.";
	if (code == LD_PRELOAD_ACCESS_PROBLEM) return "Accessing the library to preload failed.";
	if (code == LD_PRELOAD_SET_PROBLEM) return "Setting the library to preload failed.";
	if (code == CONFIG_EXEC_PROBLEM) return "Finding the location of the executable \"exec\" in the configuration file failed.";
	if (code == EXEC_FIND_PROBLEM) return "Finding the executable failed.";
	if (code == EXEC_TYPE_PROBLEM) return "Ensuring the executable is a file failed.";
	if (code == EXEC_ACCESS_PROBLEM) return "Accessing the executable failed.";
	if (code == EXEC_SIZE_PROBLEM) return "Verifying the size of the executable failed.";
	if (code == EXEC_READ_PROBLEM) return "Reading the executable failed.";
	if (code == EXEC_HASH_PROBLEM) return "Verifying the hash code of the executable failed.";
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
	if (code == CONFIG_SHM_PROBLEM) return "Finding the location of the shared memory \"shm\" in the configuration file failed. The default location will be assumed.";
	if (code == SHM_OPEN_PROBLEM) return "Opening the shared memory failed.";
	if (code == SHM_TRUNCATE_PROBLEM) return "Truncating the shared memory failed.";
	if (code == SHM_MAP_PROBLEM) return "Mapping the shared memory failed.";
	if (code == CONFIG_ROW_PROBLEM) return "Finding the height of the terminal \"rows\" in the configuration file failed. The default height will be assumed.";
	if (code == CONFIG_COL_PROBLEM) return "Finding the width of the terminal \"cols\" in the configuration file failed. The default width will be assumed.";
	if (code == CONFIG_STATE_PROBLEM) return "Finding the amount of save states \"states\" in the configuration file failed. The default amount will be assumed.";
	if (code == STATE_AMOUNT_PROBLEM) return "The amount of save states must be positive. The minimum amount will be assumed.";
	if (code == CONFIG_ITERATOR_PROBLEM) return "Finding the iterator string \"iterator\" in the configuration file failed. The default string will be assumed.";
	if (code == CONFIG_INPUT_PROBLEM) return "Finding the location of the input file \"input\" in the configuration file failed. The default location will be assumed.";
	if (code == INPUT_FIND_PROBLEM) return "Finding the input file failed.";
	if (code == INPUT_ACCESS_PROBLEM) return "Accessing the input file failed.";
	if (code == CONFIG_OUTPUT_PROBLEM) return "Finding the location of the output file \"output\" in the configuration file failed. The default location will be assumed.";
	if (code == OUTPUT_OVERWRITE_PROBLEM) return "The output file already exists and may be overwritten.";
	if (code == OUTPUT_REPLACEMENT_PROBLEM) return "The location of the output file contains more than one replacement characters. Only the first one will be replaced.";
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

/**
Propagates an error.

Returns the error code of an error if one occurs:
<pre>
problem_t method {
	PROPAGATE(another_method());
	yet_another_method();
	return NO_PROBLEM;
}
</pre>

@param code The error code.
@return The error code.
**/
#undef PROPAGATE
#define PROPAGATE(code) do {\
		const problem_t PROPAGATE_code = code;\
		if (PROPAGATE_code != NO_PROBLEM) return PROPAGATE_code;\
	} while (FALSE)

#endif
