#pragma once
#define _CRT_SECURE_NO_WARNINGS
#ifndef HEADER_6
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

typedef struct found {
	char* file_name;
	int string_num, symbol_num;
} found;

typedef enum find_substr_statuses {
	find_substr_malloc_error,
	find_substr_realloc_error,
	find_substr_open_file_error,
	find_substr_incorrect_substr,
	find_substr_ok
} find_substr_statuses;

/// <summary>
/// Reads a string from stdin. Memory for string is allocated in function
/// </summary>
/// <param name="substring"> - where to read</param>
/// <param name="substring_len"> - resulting length of the string</param>
void read_substring(char** substring);

/// <summary>
/// Finds substring in files enetered after <count>
/// </summary>
/// <param name="substr"> - a substring to search for</param>
/// <param name="substr_len"> - a substring's length</param>
/// <param name="found_substrs"> - an array of data to store information about found in files substring</param>
/// <param name="found_count"> - count of found occasions</param>
/// <param name="count"> - count of filenames</param>
/// <param name=""> - file to search where</param>
/// <returns>find_substr_statuses</returns>
find_substr_statuses find_substr(char* substr, found** found_substrs, int* found_count, int count, ...);

/// <summary>
/// Prints an array of information about found substring in files
/// </summary>
/// <param name="found_substrs"></param>
/// <param name="found_count"></param>
void print_found(found* found_substrs, int found_count);


#endif // HEADER_6
