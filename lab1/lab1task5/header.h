#pragma once
#ifndef HEADER_5

#include <stdio.h>
#include <string.h>

typedef enum fi_statuses {
	fi_incorrect_file_name,
	fi_allocation_error,
	fi_open_error,
	fi_ok
} fi_statuses;

typedef enum cin_statuses {
	cin_open_error,
	cin_ok
} cin_statuses;

typedef enum arg_statuses {
	arg_open_error,
	arg_ok
} arg_statuses;

/// <summary>
/// The function concatenates files which names are passed in file
/// </summary>
/// <param name="f_info_name"> - name of file which passes names of files to concatenate</param>
/// <returns>fi_open_error || fi_ok</returns>
fi_statuses fi(char* f_info_name);

/// <summary>
/// The function concatenates files which names are entered by user
/// </summary>
/// <returns>cin_open_error || cin_ok</returns>
cin_statuses cin();

/// <summary>
/// The function concatenates files which names are passed as command line arguments
/// </summary>
/// <param name="argc"> - count of arguments passed in command line</param>
/// <param name="argv"> - array of arguments passed in command line</param>
/// <returns>arg_open_error || arg_ok</returns>
arg_statuses arg(int argc, char** argv);

#endif // !HEADER_5
