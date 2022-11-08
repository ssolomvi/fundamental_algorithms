#pragma once
#ifndef HEADER_7
#include <stdlib.h>
#include <float.h>

typedef enum gen_arr_statuses {
	gen_arr_malloc_error,
	gen_arr_ok
} gen_arr_statuses;

/// <summary>
/// Generates array of type double with random whole and double numbers
/// </summary>
/// <param name="array"> - array to generate</param>
/// <param name="size"> - size of array to generate</param>
/// <returns>gen_arr_statuses</returns>
gen_arr_statuses gen_arr(double** array, int size);

/// <summary>
/// Finds min and max values of array of double type and their indexes
/// </summary>
/// <param name="array"> - array where to search min max values</param>
/// <param name="arr_size"> - size of array where to search</param>
/// <param name="min"> - min value of array</param>
/// <param name="max"> - max value of array</param>
/// <param name="ind_min"> - index of min value of array</param>
/// <param name="ind_max"> - index of max value of array</param>
/// <returns>min_max_statuses</returns>
void min_max(double* array, int arr_size, double* min, double* max, int* ind_min, int* ind_max);

void pr_array(double* array, int arr_size);

#endif // !HEADER_7
