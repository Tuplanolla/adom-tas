/**
Handles errors.

Errors are major problems.
Warnings are minor problems.
Notes are not problems.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef PROB_C
#define PROB_C

#include "prob.h"//problem_d, *_PROBLEM

/**
Converts an error code to an error message.

@param code The error code.
@return The error message.
**/
const char * problem_message(const problem_d code) {
	switch (code) {
		/*
		*.c
		*/
		case NO_PROBLEM: return "Nothing failed.";
		case ASSERT_PROBLEM: return "Evaluating an assertion failed.";
		case NULL_PROBLEM: return "Dereferencing a null pointer failed.";
		case MALLOC_PROBLEM: return "Allocating memory failed.";

		/*
		main.c
		*/
		case TEMPORARY_UNLINK_PROBLEM: return "Removing at least one of the temporary files failed.";
		case CONFIG_OPEN_PROBLEM: return "Opening the configuration file for writing failed.";
		case CONFIG_WRITE_PROBLEM: return "Writing the configuration file failed.";
		case CONFIG_CLOSE_PROBLEM: return "Closing the configuration file failed.";
		case PROCESS_UNLINK_PROBLEM: return "Removing the process lock file failed.";
		case KEYBIND_OPEN_PROBLEM: return "Opening the keybinding file for writing failed.";
		case KEYBIND_WRITE_PROBLEM: return "Writing the keybinding file failed.";
		case KEYBIND_CLOSE_PROBLEM: return "Closing the keybinding file failed.";
		case VERSION_OPEN_PROBLEM: return "Opening the version file for reading failed.";
		case VERSION_READ_PROBLEM: return "Reading the version file failed.";
		case VERSION_CLOSE_PROBLEM: return "Closing the version file failed.";
		case VERSION_MISMATCH_PROBLEM: return "Verifying the version of the executable failed.";
		case ERROR_UNLINK_PROBLEM: return "Removing the error log file failed.";
		case COUNT_OPEN_PROBLEM: return "Opening the count file for writing failed.";
		case COUNT_WRITE_PROBLEM: return "Writing the count file failed.";
		case COUNT_CLOSE_PROBLEM: return "Closing the count file failed.";
		case LD_PRELOAD_SETENV_PROBLEM: return "Setting the library to preload in the environment variable failed.";
		case EXEC_PROBLEM: return "Launching the executable failed.";

		/*
		cfg.c
		*/
		case CONFIGURATION_OPEN_PROBLEM: return "Opening the configuration file for writing failed.";
		case CONFIGURATION_WRITE_PROBLEM: return "Writing the configuration file failed.";
		case CONFIGURATION_CLOSE_PROBLEM: return "Closing the configuration file failed.";
		case CONFIGURATION_STAT_PROBLEM: return "Finding the configuration file failed. It will be created from a template.";
		case CONFIGURATION_PARSE_PROBLEM: return "Parsing the configuration file failed.";
		case HOME_CONFIG_PROBLEM: return "Finding the location of the home directory in the configuration file failed.";
		case HOME_GETENV_PROBLEM: return "Finding the location of the home directory in the environment variable failed.";
		case HOME_GETPWUID_PROBLEM: return "Finding the location of the home directory in the environment variable failed. It will be guessed.";
		case HOME_STAT_PROBLEM: return "Accessing the home directory failed. Shell expansions will be unavailable.";
		case EXEC_DATA_CONFIG_PROBLEM: return "Finding the location of the executable's data directory in the configuration file failed. It will be guessed.";
		case EXEC_DATA_HOME_PROBLEM: return "Guessing the location of the executable's data directory failed.";
		case EXEC_DATA_STAT_PROBLEM: return "Accessing the executable's data directory failed.";
		case EXEC_TEMPORARY_STAT_PROBLEM: return  "Accessing at least one of the executable's temporary file failed.";

		case LD_PRELOAD_CONFIG_PROBLEM: return "Finding the location of the library to preload in the configuration file failed.";
		case LD_PRELOAD_GETENV_PROBLEM: return "Finding the location of the library to preload in the environment variable failed. It will be guessed.";
		case LD_PRELOAD_STAT_PROBLEM: return "Accessing the library to preload failed.";
		case EXEC_CONFIG_PROBLEM: return "Finding the location of the executable in the configuration file failed. It will be guessed.";
		case EXEC_STAT_PROBLEM: return "Accessing the executable failed.";
		case EXEC_TYPE_PROBLEM: return "Ensuring the executable is a file failed.";
		case EXEC_PERMISSION_PROBLEM: return "Checking the permissions of the executable failed.";
		case EXEC_SIZE_PROBLEM: return "Verifying the size of the executable failed.";
		case EXEC_OPEN_PROBLEM: return "Opening the executable for reading failed.";
		case EXEC_READ_PROBLEM: return "Reading the executable failed.";
		case EXEC_HASH_PROBLEM: return "Verifying the hash code of the executable failed.";
		case EXEC_CLOSE_PROBLEM: return "Closing the executable failed.";
		case EXEC_CONFIG_STAT_PROBLEM: return "Accessing the executable's configuration file failed.";
		case EXEC_PROCESS_STAT_PROBLEM: return "Accessing the executable's process lock file failed.";
		case EXEC_KEYBIND_STAT_PROBLEM: return "Accessing the executable's keybinding file failed.";
		case EXEC_VERSION_STAT_PROBLEM: return "Accessing the executable's version file failed.";
		case EXEC_ERROR_STAT_PROBLEM: return "Accessing the executable's error log file failed.";
		case EXEC_COUNT_STAT_PROBLEM: return "Accessing the executable's count file failed.";

		case LIBC_CONFIG_PROBLEM: return "Finding the location of the C standard library in the configuration file failed. It will be guessed.";
		case LIBC_STAT_PROBLEM: return "Accessing the C standard library failed.";
		case LIBNCURSES_CONFIG_PROBLEM: return "Finding the location of the New Cursor Optimization library in the configuration file failed. It will be guessed.";
		case LIBNCURSES_STAT_PROBLEM: return "Accessing the New Cursor Optimization library failed.";
		case ENFORCE_CONFIG_PROBLEM: return "Finding whether configurations should be enforced in the configuration file failed. The default value will be assumed.";
		case ROW_CONFIG_PROBLEM: return "Finding the height of the terminal in the configuration file failed. The default value will be assumed.";
		case ROW_AMOUNT_PROBLEM: return "Setting the height of the terminal failed. The closest estimate will be used.";
		case COL_CONFIG_PROBLEM: return "Finding the width of the terminal in the configuration file failed. The default value will be assumed.";
		case COL_AMOUNT_PROBLEM: return "Setting the width of the terminal failed. The closest estimate will be used.";
		case STATE_CONFIG_PROBLEM: return "Finding the amount of save states in the configuration file failed. The default value will be assumed.";
		case STATE_AMOUNT_PROBLEM: return "Setting the amount of save states failed. The closest estimate will be used.";
		case SHM_CONFIG_PROBLEM: return "Finding the location of the shared memory segment in the configuration file failed. The default value will be assumed.";
		case SHM_STAT_PROBLEM: return "Accessing the shared memory segment failed.";
		case GENERATIONS_CONFIG_PROBLEM: return "Finding the amount of generated characters in the configuration file failed. The default value will be assumed.";
		case SQL_CONFIG_PROBLEM: return "Finding whether the save-quit-load emulation is enabled in the configuration file failed. The default value will be assumed.";
		case PRESERVE_CONFIG_PROBLEM: return "Finding whether save states should be preserved upon loading in the configuration file failed. The default value will be assumed.";
		case AUTOPLAY_CONFIG_PROBLEM: return "Finding the playback mode in the configuration file failed. The default value will be assumed.";
		case MONOCHROME_CONFIG_PROBLEM: return "Finding the color mode in the configuration file failed. The default value will be assumed.";
		case ITERATOR_CONFIG_PROBLEM: return "Finding the iterator string in the configuration file failed. The default value will be assumed.";
		case INPUT_CONFIG_PROBLEM: return "Finding the location of the input file in the configuration file failed. The default value will be assumed.";
		case INPUT_STAT_PROBLEM: return "The input file doesn't exist.";
		case OUTPUT_CONFIG_PROBLEM: return "Finding the locations of the output files in the configuration file failed. The default values will be assumed.";
		case OUTPUT_STAT_PROBLEM: return "At least one of the output files already exists. It may be overwritten.";
		case ERROR_CONFIG_PROBLEM: return "Finding the location of the error log in the configuration file failed. The default value will be assumed.";
		case ERROR_STAT_PROBLEM: return "Accessing the error log failed.";
		case ERROR_OPEN_PROBLEM: return "Opening the error log for writing failed.";
		case ERROR_CLOSE_PROBLEM: return "Closing the error log file failed.";
		case WARNING_CONFIG_PROBLEM: return "Finding the location of the warning log in the configuration file failed. The default value will be assumed.";
		case WARNING_STAT_PROBLEM: return "Accessing the warning log failed.";
		case WARNING_OPEN_PROBLEM: return "Opening the warning log for writing failed.";
		case WARNING_CLOSE_PROBLEM: return "Closing the warning log file failed.";
		case NOTE_CONFIG_PROBLEM: return "Finding the location of the note log in the configuration file failed. The default value will be assumed.";
		case NOTE_STAT_PROBLEM: return "Accessing the note log failed.";
		case NOTE_OPEN_PROBLEM: return "Opening the note log for writing failed.";
		case NOTE_CLOSE_PROBLEM: return "Closing the note log file failed.";
		case CALL_CONFIG_PROBLEM: return "Finding the location of the call log in the configuration file failed. The default value will be assumed.";
		case CALL_STAT_PROBLEM: return "Accessing the call log failed.";
		case CALL_OPEN_PROBLEM: return "Opening the call log for writing failed.";
		case CALL_CLOSE_PROBLEM: return "Closing the call log file failed.";
		case LOG_CHANGE_PROBLEM: return "The log streams changed. Further messages will be redirected.";

		/*
		shm.c
		*/
		case SHM_MALLOC_PROBLEM: return "Allocating shared memory failed.";
		case SHM_KEY_PROBLEM: return "Generating a shared memory key failed.";
		case SHM_GET_PROBLEM: return "Finding the shared memory segment failed.";
		case SHM_ATTACH_PROBLEM: return "Attaching the shared memory segment failed.";
		case SHM_DETACH_PROBLEM: return "Detaching the shared memory segment failed.";
		case SHM_REMOVE_PROBLEM: return "Removing the shared memory segment failed.";

		/*
		put.c
		*/
		case INPUT_OPEN_PROBLEM: return "Opening the input file for reading failed.";
		case INPUT_READ_PROBLEM: return "Reading the input file failed.";
		case INPUT_CLOSE_PROBLEM: return "Closing the input file failed.";
		case OUTPUT_OPEN_PROBLEM: return "Opening the output file for writing failed.";
		case OUTPUT_WRITE_PROBLEM: return "Writing the output file failed.";
		case OUTPUT_CLOSE_PROBLEM: return "Closing the output file failed.";

		/*
		lib.c
		*/
		case LIBC_DLOPEN_PROBLEM: return "Opening the C standard library for linking failed.";
		case LIBC_DLSYM_PROBLEM: return "Reading at least one of the symbols from the C standard library failed.";
		case LIBNCURSES_DLOPEN_PROBLEM: return "Opening the New Cursor Optimization library for linking failed.";
		case LIBNCURSES_DLSYM_PROBLEM: return "Reading at least one of the symbols from the New Cursor Optimization library failed.";
		case LD_PRELOAD_UNSETENV_PROBLEM: return "Unsetting the library to preload in the environment variable failed.";
		case LIBC_DLCLOSE_PROBLEM: return "Closing the C standard library failed.";
		case LIBNCURSES_DLCLOSE_PROBLEM: return "Closing the New Cursor Optimization library failed.";

		/*
		fork.c
		*/
		case FORK_PROBLEM: return "Creating a new process failed.";

		/*
		gui.c
		*/
		case INIT_PAIR_PROBLEM: return "Creating a new color pair failed.";
		case NEWWIN_PROBLEM: return "Creating a new window failed.";
		case DELWIN_PROBLEM: return "Removing new window failed.";

		/*
		asm.c
		*/
		case ASM_MPROTECT_PROBLEM: return "Unprotecting the instructions to overwrite failed.";
		case ASM_MEMCMP_PROBLEM: return "Finding the instructions to overwrite failed.";

		default: return "Converting an error code to an error message failed.";
	}
}

#endif
