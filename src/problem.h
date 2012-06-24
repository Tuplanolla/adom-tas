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
	NO_PROBLEM,

	CONFIG_OPEN_PROBLEM,
	CONFIG_WRITE_PROBLEM,
	CONFIG_CLOSE_PROBLEM,
	CONFIG_STAT_PROBLEM,
	CONFIG_PARSE_PROBLEM,
	HOME_CONFIG_PROBLEM,
	HOME_GETENV_PROBLEM,
	HOME_GETPWUID_PROBLEM,
	HOME_STAT_PROBLEM,
	LD_PRELOAD_CONFIG_PROBLEM,
	LD_PRELOAD_GETENV_PROBLEM,
	LD_PRELOAD_STAT_PROBLEM,
	LD_PRELOAD_SETENV_PROBLEM,
	EXECUTABLE_CONFIG_PROBLEM,
	EXECUTABLE_STAT_PROBLEM,
	EXECUTABLE_TYPE_PROBLEM,
	EXECUTABLE_PERMISSION_PROBLEM,
	EXECUTABLE_SIZE_PROBLEM,
	EXECUTABLE_OPEN_PROBLEM,
	EXECUTABLE_READ_PROBLEM,
	EXECUTABLE_HASH_PROBLEM,
	EXECUTABLE_CLOSE_PROBLEM,
	EXECUTABLE_DATA_CONFIG_PROBLEM,
	EXECUTABLE_DATA_HOME_PROBLEM,
	EXECUTABLE_DATA_STAT_PROBLEM,
	EXECUTABLE_PROCESS_STAT_PROBLEM,
	EXECUTABLE_VERSION_STAT_PROBLEM,
	EXECUTABLE_COUNT_STAT_PROBLEM,
	EXECUTABLE_KEYBIND_STAT_PROBLEM,
	EXECUTABLE_CONFIG_STAT_PROBLEM,
	LIBC_CONFIG_PROBLEM,
	LIBC_STAT_PROBLEM,
	LIBNCURSES_CONFIG_PROBLEM,
	LIBNCURSES_STAT_PROBLEM,
	ROW_CONFIG_PROBLEM,
	ROW_AMOUNT_PROBLEM,
	COL_CONFIG_PROBLEM,
	COL_AMOUNT_PROBLEM,
	STATE_CONFIG_PROBLEM,
	STATE_AMOUNT_PROBLEM,
	SHM_CONFIG_PROBLEM,
	ITERATOR_CONFIG_PROBLEM,
	INPUT_CONFIG_PROBLEM,
	INPUT_STAT_PROBLEM,
	INPUT_OPEN_PROBLEM,
	OUTPUT_CONFIG_PROBLEM,
	OUTPUT_ITERATOR_PROBLEM,
	OUTPUT_STAT_PROBLEM,
	OUTPUT_OPEN_PROBLEM,
	ERROR_CONFIG_PROBLEM,
	ERROR_STAT_PROBLEM,
	ERROR_OPEN_PROBLEM,
	LOG_CHANGE_PROBLEM,

	COUNT_OPEN_PROBLEM,
	COUNT_WRITE_PROBLEM,
	COUNT_CLOSE_PROBLEM,
	VERSION_OPEN_PROBLEM,
	VERSION_READ_PROBLEM,
	VERSION_CLOSE_PROBLEM,
	VERSION_MISMATCH_PROBLEM,
	PROCESS_UNLINK_PROBLEM,
	EXEC_PROBLEM,

	LIBC_DLOPEN_PROBLEM,
	LIBNCURSES_DLOPEN_PROBLEM,
	LD_PRELOAD_UNSETENV_PROBLEM,

	SHM_KEY_PROBLEM,
	SHM_GET_PROBLEM,
	SHM_ATTACH_PROBLEM,
	SHM_DETACH_PROBLEM,
	SHM_REMOVE_PROBLEM,

	FORK_PROBLEM,

	ASM_PROBLEM,
	MPROTECT_PROBLEM
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
LIBC_CONFIG_PROBLEM "Finding the location of the C standard library in the configuration file failed. It will be guessed.";
LIBC_STAT_PROBLEM "Accessing the C standard library failed.";
LIBNCURSES_CONFIG_PROBLEM "Finding the location of the New Cursor Optimization library in the configuration file failed. It will be guessed.";
LIBNCURSES_STAT_PROBLEM "Accessing the New Cursor Optimization library failed.";
ROW_CONFIG_PROBLEM "Finding the height of the terminal in the configuration file failed. The default value will be assumed.";
ROW_AMOUNT_PROBLEM "Setting the height of the terminal failed. The closest estimate will be used.";
COL_CONFIG_PROBLEM "Finding the width of the terminal in the configuration file failed. The default value will be assumed.";
COL_AMOUNT_PROBLEM "Setting the width of the terminal failed. The closest estimate will be used.";
STATE_CONFIG_PROBLEM "Finding the amount of save states in the configuration file failed. The default value will be assumed.";
STATE_AMOUNT_PROBLEM "Setting the amount of save states failed. The closest estimate will be used.";
SHM_CONFIG_PROBLEM "Finding the location of the shared memory segment in the configuration file failed. The default value will be assumed.";
ITERATOR_CONFIG_PROBLEM "Finding the iterator string in the configuration file failed. The default value will be assumed.";
INPUT_CONFIG_PROBLEM "Finding the location of the input file in the configuration file failed. The default value will be assumed.";
INPUT_STAT_PROBLEM
INPUT_OPEN_PROBLEM
OUTPUT_CONFIG_PROBLEM "Finding the location of the output files in the configuration file failed. The default values will be assumed.";
OUTPUT_ITERATOR_PROBLEM
OUTPUT_STAT_PROBLEM
OUTPUT_OPEN_PROBLEM
ERROR_CONFIG_PROBLEM
ERROR_STAT_PROBLEM
ERROR_OPEN_PROBLEM
LOG_CHANGE_PROBLEM "The log locations changed.";

COUNT_OPEN_PROBLEM "Opening the count file for writing failed.";
COUNT_WRITE_PROBLEM "Writing the count file failed.";
COUNT_CLOSE_PROBLEM "Closing the count file failed.";
VERSION_OPEN_PROBLEM "Opening the version file for reading failed.";
VERSION_READ_PROBLEM "Reading the version file failed.";
VERSION_CLOSE_PROBLEM "Closing the version file failed.";
VERSION_MISMATCH_PROBLEM "Verifying the version of the executable failed.";
PROCESS_UNLINK_PROBLEM "Removing the process file failed.";
EXEC_PROBLEM "Launching the executable failed.";

LIBC_DLOPEN_PROBLEM
LIBNCURSES_DLOPEN_PROBLEM
LD_PRELOAD_UNSETENV_PROBLEM

SHM_KEY_PROBLEM
SHM_GET_PROBLEM
SHM_ATTACH_PROBLEM
SHM_DETACH_PROBLEM
SHM_REMOVE_PROBLEM

FORK_PROBLEM

ASM_PROBLEM
MPROTECT_PROBLEM
*/
inline char * problem_message(const problem_t code) {
	switch (code) {
		case NO_PROBLEM: return "Nothing failed.";
		default: return "Converting an error code to an error message failed.";
	}
}

/**
Propagates an error code.

Returns the error code of an error if one occurs:
<pre>
problem_t method {
	PROPAGATE(another_method());
	yet_another_method();
	return NO_PROBLEM;
}
</pre>

@param code The error code.
**/
#undef PROPAGATE
#define PROPAGATE(code) do {\
		const problem_t PROPAGATE_code = code;\
		if (PROPAGATE_code != NO_PROBLEM) return PROPAGATE_code;\
	} while (FALSE)

#endif
