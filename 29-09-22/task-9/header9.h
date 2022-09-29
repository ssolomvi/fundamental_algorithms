#pragma once
#ifndef HEADER_9
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define eps 1e-8

typedef enum read_arr_statuses {
	read_arr_allocation_error,
	read_arr_reallocation_error,
	read_arr_incorrect_notation,
	read_arr_ok,
} read_arr_statuses;

typedef enum find_max_abs_statuses {
	find_max_abs_array_empty,
	find_max_abs_ok
} find_max_abs_statuses;

typedef enum convert_to_statuses {
	conv_malloc_error,
	conv_realloc_error,
	conv_ok
} convert_to_statuses;


/// <summary>
/// Reads an array of floats from stdin, converts them in decimal notation, "Stop" is the end of reading
/// </summary>
/// <param name="array"> - where to put numbers</param>
/// <param name="actual_size_array"> - size of array</param>
/// <param name="notation"></param>
/// <returns>read_arr_statuses</returns>
read_arr_statuses read_array(float** array, int* actual_size_array, int notation);

/// <summary>
/// Finds max |value| of an array
/// </summary>
/// <param name="array"> - array where to search</param>
/// <param name="size"> - size of array</param>
/// <param name="max"> - found max value</param>
/// <returns>find_max_abs_statuses</returns>
find_max_abs_statuses find_max_abs(float* array, int size, float* max);

/// <summary>
/// Converts a number of float type into enetered notation
/// </summary>
/// <param name="number_f"> - number to convert</param>
/// <param name="notation"></param>
/// <param name="number_str"> - converted number</param>
/// <returns>convert_to_statuses</returns>
convert_to_statuses convert(float number_f, int notation, char** number_str);

#endif // !HEADER_9
