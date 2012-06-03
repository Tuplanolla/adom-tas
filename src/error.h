/**
Provides error handling.
Problems are errors that appear in the wrapper.
Errors are major errors that appear in the loader.
Warnings are minor errors that appear in the loader.
Errors and warnings are logged if a log file is specified.
**/
#ifndef ERROR_H
#define ERROR_H

/**
Lists the error codes.
**/
enum error_e {
	NO_ERROR,
	EXECUTION_ERROR,
	SETENV_HOME_ERROR,
	SETENV_LD_PRELOAD_ERROR,
	USER_ID_ERROR,
	DLOPEN_LIBC_ERROR,
	DLOPEN_LIBNCURSES_ERROR,
	STAT_ERROR,
	WRONG_VERSION_ERROR,
	WRONG_SIZE_ERROR,
	LOG_WARNING
};
typedef enum error_e error_t;

/**
Converts an error code to an error message.
@param error The error code.
@return The error message.
**/
inline char *error_message(const error_t code) {
	if (code == NO_ERROR) return "Nothing failed.";
	else if (code == SETENV_LD_PRELOAD_ERROR) return "Setting the environment variable LD_PRELOAD failed.";
	else if (code == USER_ID_ERROR) return "Linking dynamic libraries failed due to conflicting user identifiers.";
	else if (code == DLOPEN_LIBC_ERROR) return "Loading libc failed.";
	else if (code == DLOPEN_LIBNCURSES_ERROR) return "Loading libncurses failed.";
	else if (code == STAT_ERROR) return "Reading the executable failed.";
	else if (code == WRONG_VERSION_ERROR) return "Identifying the executable by its version failed.";
	else if (code == WRONG_SIZE_ERROR) return "Identifying the executable by its size failed.";
	else if (code == LOG_WARNING) return "Failed to open the log file. Logging to stderr (standard error stream) instead.";
	else return "Failed to fail.";
}

#endif
