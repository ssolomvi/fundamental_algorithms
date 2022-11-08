#pragma once
#ifndef HEADER_8
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <float.h>
#define eps 1e-8

typedef enum read_arr_statuses {
	read_arr_malloc_error,
	read_arr_realloc_error,
	read_arr_incorrect_number,
	read_arr_ok,
} read_arr_statuses;

typedef enum odd_statuses {
	odd_realloc_error,
	odd_ok,
} odd_statuses;

typedef enum even_statuses {
	even_realloc_error,
	even_ok,
} even_statuses;

typedef enum farest_statuses {
	farest_realloc_error,
	farest_ok,
} farest_statuses;

typedef enum summa_precede_statuses {
	summa_precede_realloc_error,
	summa_precede_type_overflow,
	summa_precede_ok,
} summa_precede_statuses;

typedef enum summa_less_statuses {
	summa_less_realloc_error,
	summa_less_type_overflow,
	summa_less_ok,
} summa_less_statuses;

/// <summary>
/// Reads an array of double from file stream
/// </summary>
/// <param name="fi">- opened file stream</param>
/// <param name="array"> - an array where to read</param>
/// <param name="size"> - size of array</param>
/// <returns>read_arr_atatuses</returns>
read_arr_statuses read_arr(FILE* fi, double** array, int* size);

/// <summary>
/// Adds numbers in odd postion to new array
/// </summary>
/// <param name="array_r"> - source array</param>
/// <param name="size_r"> - size of source array</param>
/// <param name="array_add"> - new array</param>
/// <param name="size_add"> - size of new array</param>
/// <returns>odd_statuses</returns>
odd_statuses add_odd_pos(double* array_r, int size_r, double** array_add, int* size_add);

/// <summary>
/// Adds even numbers to new array, requires fmy_itoa
/// </summary>
/// <param name="array_r"> - source array</param>
/// <param name="size_r"> - size of source array</param>
/// <param name="array_add"> - new array</param>
/// <param name="size_add"> - size of new array</param>
/// <returns>even_statuses</returns>
even_statuses add_even(double* array_r, int size_r, double** array_add, int* size_add);

/// <summary>
/// Adds farest element from current one (in means of value)
/// </summary>
/// <param name="array_r"> - source array</param>
/// <param name="size_r"> - size of source array</param>
/// <param name="array_add"> - new array</param>
/// <param name="size_add"> - size of new array</param>
/// <param name="ind_current"> - index of current element</param>
/// <returns>farest_statuses</returns>
farest_statuses add_farest_abs(double* array_r, int size_r, double** array_add, int* size_add, int ind_current);

/// <summary>
/// Adds summa of precede elements for current one
/// </summary>
/// <param name="array_r"> - source array</param>
/// <param name="size_r"> - size of source array</param>
/// <param name="array_add"> - new array</param>
/// <param name="size_add"> - size of new array</param>
/// <param name="ind_current"> - index of current element</param>
/// <returns>summa_precede_statuses</returns>
summa_precede_statuses add_summa_precede(double* array_r, int size_r, double** array_add, int* size_add, int ind_current);

/// <summary>
/// Adds summa of elements which are less than current one
/// </summary>
/// <param name="array_r"> - source array</param>
/// <param name="size_r"> - size of source array</param>
/// <param name="array_add"> - new array</param>
/// <param name="size_add"> - size of new array</param>
/// <param name="ind_current"> - index of current element</param>
/// <returns>summa_less_statuses</returns>
summa_less_statuses add_summa_less(double* array_r, int size_r, double** array_add, int* size_add, int ind_current);

void print_array(double* array, int size);

#endif // HEADER_8
