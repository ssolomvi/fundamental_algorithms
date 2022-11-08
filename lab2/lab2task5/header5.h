#pragma once
#ifndef HEADER_5
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>
#include <string.h>
#define eps 1e-8

typedef enum gen_arr_statuses {
	gen_arr_malloc_error,
	gen_arr_ok
} gen_arr_statuses;

typedef enum make_uniq_statuses {
	make_uniq_malloc_error,
	make_uniq_realloc_error,
	make_uniq_ok
} make_uniq_statuses;

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
/// <param name="ind_min"> - index of min value of array</param>
/// <param name="ind_max"> - index of max value of array</param>
/// <returns></returns>
void min_max(double* array, int arr_size, int* ind_min, int* ind_max);

void swap(double* num1, double* num2);

/// <summary>
/// Makes a new array containing only unique values from initial array
/// </summary>
/// <param name="init_array"> - initial array</param>
/// <param name="size"> - size of an initial array</param>
/// <param name="new_array"></param>
/// <param name="size_unique"> - size of the new array</param>
/// <returns>make_uniq_statuses</returns>
make_uniq_statuses make_uniq(double* init_array, int size, double** new_array, int* size_unique);

void print_array(double* array, int arr_size);

#endif // HEADER_5
