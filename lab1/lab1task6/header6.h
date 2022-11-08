#pragma once
#ifndef HEADER_6
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

typedef enum read_number_statuses {
	read_number_alloc_error,
	read_number_realloc_error,
	read_number_incorrect_number,
	read_number_ok,
} read_number_statuses;

typedef enum my_itoa_statuses {
	my_itoa_malloc_error,
	my_itoa_realloc_error,
	my_itoa_ok
} my_itoa_statuses;

/// <summary>
/// Reads one number from file and writes it down the string. Memory for string is allocated.
/// </summary>
/// <param name="number_x"> - where to write a read number</param>
/// <param name="file_r"> - from where to read</param>
/// <returns>read_number_statuses</returns>
read_number_statuses read_number(char** number_x, FILE* file_r);

int get_notation(char* number);

/// <summary>
/// Convert number from int to string
/// </summary>
/// <param name="number"> - number to convert</param>
/// <param name="number_str"> - converted number</param>
/// <returns>my_itoa_statuses</returns>
my_itoa_statuses my_itoa(long long number, char** number_str);

long long to_decimal(char* number_str, int notation);


#endif // HEADER_6