/**
Handles errors.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef PROBLEM_C
#define PROBLEM_C

#include "problem.h"//problem_t, *_PROBLEM

/**
Converts an error code to an error message.

@param code The error code.
@return The error message.
**/
/*
NO_PROBLEM "Nothing failed.";
CAUSALITY_PROBLEM "Failing failed."

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
SHM_STAT_PROBLEM "Accessing the shared memory segment failed.";
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
^
INPUT_CLOSE_PROBLEM
OUTPUT_CLOSE_PROBLEM
ERROR_CLOSE_PROBLEM
WARNING_CLOSE_PROBLEM
NOTE_CLOSE_PROBLEM
CALL_CLOSE_PROBLEM

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

ASM_MEMCMP_PROBLEM
ASM_MPROTECT_PROBLEM
*/
const char * problem_message(const problem_t code) {
	switch (code) {
		case NO_PROBLEM: return "Nothing failed.";
		case CAUSALITY_PROBLEM: return "CAUSALITY_PROBLEM";
		case CONFIG_OPEN_PROBLEM: return "CONFIG_OPEN_PROBLEM";
		case CONFIG_WRITE_PROBLEM: return "CONFIG_WRITE_PROBLEM";
		case CONFIG_CLOSE_PROBLEM: return "CONFIG_CLOSE_PROBLEM";
		case CONFIG_STAT_PROBLEM: return "CONFIG_STAT_PROBLEM";
		case CONFIG_PARSE_PROBLEM: return "CONFIG_PARSE_PROBLEM";
		case HOME_CONFIG_PROBLEM: return "HOME_CONFIG_PROBLEM";
		case HOME_GETENV_PROBLEM: return "HOME_GETENV_PROBLEM";
		case HOME_GETPWUID_PROBLEM: return "HOME_GETPWUID_PROBLEM";
		case HOME_STAT_PROBLEM: return "HOME_STAT_PROBLEM";
		case LD_PRELOAD_CONFIG_PROBLEM: return "LD_PRELOAD_CONFIG_PROBLEM";
		case LD_PRELOAD_GETENV_PROBLEM: return "LD_PRELOAD_GETENV_PROBLEM";
		case LD_PRELOAD_STAT_PROBLEM: return "LD_PRELOAD_STAT_PROBLEM";
		case LD_PRELOAD_SETENV_PROBLEM: return "LD_PRELOAD_SETENV_PROBLEM";
		case EXECUTABLE_CONFIG_PROBLEM: return "EXECUTABLE_CONFIG_PROBLEM";
		case EXECUTABLE_STAT_PROBLEM: return "EXECUTABLE_STAT_PROBLEM";
		case EXECUTABLE_TYPE_PROBLEM: return "EXECUTABLE_TYPE_PROBLEM";
		case EXECUTABLE_PERMISSION_PROBLEM: return "EXECUTABLE_PERMISSION_PROBLEM";
		case EXECUTABLE_SIZE_PROBLEM: return "EXECUTABLE_SIZE_PROBLEM";
		case EXECUTABLE_OPEN_PROBLEM: return "EXECUTABLE_OPEN_PROBLEM";
		case EXECUTABLE_READ_PROBLEM: return "EXECUTABLE_READ_PROBLEM";
		case EXECUTABLE_HASH_PROBLEM: return "EXECUTABLE_HASH_PROBLEM";
		case EXECUTABLE_CLOSE_PROBLEM: return "EXECUTABLE_CLOSE_PROBLEM";
		case EXECUTABLE_DATA_CONFIG_PROBLEM: return "EXECUTABLE_DATA_CONFIG_PROBLEM";
		case EXECUTABLE_DATA_HOME_PROBLEM: return "EXECUTABLE_DATA_HOME_PROBLEM";
		case EXECUTABLE_DATA_STAT_PROBLEM: return "EXECUTABLE_DATA_STAT_PROBLEM";
		case EXECUTABLE_PROCESS_STAT_PROBLEM: return "EXECUTABLE_PROCESS_STAT_PROBLEM";
		case EXECUTABLE_VERSION_STAT_PROBLEM: return "EXECUTABLE_VERSION_STAT_PROBLEM";
		case EXECUTABLE_COUNT_STAT_PROBLEM: return "EXECUTABLE_COUNT_STAT_PROBLEM";
		case EXECUTABLE_KEYBIND_STAT_PROBLEM: return "EXECUTABLE_KEYBIND_STAT_PROBLEM";
		case EXECUTABLE_CONFIG_STAT_PROBLEM: return "EXECUTABLE_CONFIG_STAT_PROBLEM";
		case LIBC_CONFIG_PROBLEM: return "LIBC_CONFIG_PROBLEM";
		case LIBC_STAT_PROBLEM: return "LIBC_STAT_PROBLEM";
		case LIBNCURSES_CONFIG_PROBLEM: return "LIBNCURSES_CONFIG_PROBLEM";
		case LIBNCURSES_STAT_PROBLEM: return "LIBNCURSES_STAT_PROBLEM";
		case ROW_CONFIG_PROBLEM: return "ROW_CONFIG_PROBLEM";
		case ROW_AMOUNT_PROBLEM: return "ROW_AMOUNT_PROBLEM";
		case COL_CONFIG_PROBLEM: return "COL_CONFIG_PROBLEM";
		case COL_AMOUNT_PROBLEM: return "COL_AMOUNT_PROBLEM";
		case STATE_CONFIG_PROBLEM: return "STATE_CONFIG_PROBLEM";
		case STATE_AMOUNT_PROBLEM: return "STATE_AMOUNT_PROBLEM";
		case SHM_CONFIG_PROBLEM: return "SHM_CONFIG_PROBLEM";
		case SHM_STAT_PROBLEM: return "SHM_STAT_PROBLEM";
		case ITERATOR_CONFIG_PROBLEM: return "ITERATOR_CONFIG_PROBLEM";
		case INPUT_CONFIG_PROBLEM: return "INPUT_CONFIG_PROBLEM";
		case INPUT_STAT_PROBLEM: return "INPUT_STAT_PROBLEM";
		case INPUT_OPEN_PROBLEM: return "INPUT_OPEN_PROBLEM";
		case OUTPUT_CONFIG_PROBLEM: return "OUTPUT_CONFIG_PROBLEM";
		case OUTPUT_ITERATOR_PROBLEM: return "OUTPUT_ITERATOR_PROBLEM";
		case OUTPUT_STAT_PROBLEM: return "OUTPUT_STAT_PROBLEM";
		case OUTPUT_OPEN_PROBLEM: return "OUTPUT_OPEN_PROBLEM";
		case ERROR_CONFIG_PROBLEM: return "ERROR_CONFIG_PROBLEM";
		case ERROR_STAT_PROBLEM: return "ERROR_STAT_PROBLEM";
		case ERROR_OPEN_PROBLEM: return "ERROR_OPEN_PROBLEM";
		case LOG_CHANGE_PROBLEM: return "LOG_CHANGE_PROBLEM";
		case COUNT_OPEN_PROBLEM: return "COUNT_OPEN_PROBLEM";
		case COUNT_WRITE_PROBLEM: return "COUNT_WRITE_PROBLEM";
		case COUNT_CLOSE_PROBLEM: return "COUNT_CLOSE_PROBLEM";
		case VERSION_OPEN_PROBLEM: return "VERSION_OPEN_PROBLEM";
		case VERSION_READ_PROBLEM: return "VERSION_READ_PROBLEM";
		case VERSION_CLOSE_PROBLEM: return "VERSION_CLOSE_PROBLEM";
		case VERSION_MISMATCH_PROBLEM: return "VERSION_MISMATCH_PROBLEM";
		case PROCESS_UNLINK_PROBLEM: return "PROCESS_UNLINK_PROBLEM";
		case EXEC_PROBLEM: return "EXEC_PROBLEM";
		case LIBC_DLOPEN_PROBLEM: return "LIBC_DLOPEN_PROBLEM";
		case LIBNCURSES_DLOPEN_PROBLEM: return "LIBNCURSES_DLOPEN_PROBLEM";
		case LD_PRELOAD_UNSETENV_PROBLEM: return "LD_PRELOAD_UNSETENV_PROBLEM";
		case SHM_KEY_PROBLEM: return "SHM_KEY_PROBLEM";
		case SHM_GET_PROBLEM: return "SHM_GET_PROBLEM";
		case SHM_ATTACH_PROBLEM: return "SHM_ATTACH_PROBLEM";
		case SHM_DETACH_PROBLEM: return "SHM_DETACH_PROBLEM";
		case SHM_REMOVE_PROBLEM: return "SHM_REMOVE_PROBLEM";
		case FORK_PROBLEM: return "FORK_PROBLEM";
		case ASM_MEMCMP_PROBLEM: return "ASM_MEMCMP_PROBLEM";
		case ASM_MPROTECT_PROBLEM: return "ASM_MPROTECT_PROBLEM";
		default: return "Converting an error code to an error message failed.";
	}
}

#endif
