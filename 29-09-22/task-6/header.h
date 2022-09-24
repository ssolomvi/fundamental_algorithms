#pragma once
#ifndef HEADER
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

typedef enum read_number_statuses {
	read_number_alloc_error,
	read_number_realloc_error,
	read_number_incorrect_number,
	read_number_ok,
} read_number_statuses;


/// <summary>
/// Reads one number from file and writes it down the string. Memory for string is allocated.
/// </summary>
/// <param name="number_x"> - where to write a read number</param>
/// <param name="file_r"> - from where to read</param>
/// <param name="is_double"> - flag saying if the read number is double</param>
/// <returns>read_number_statuses</returns>
read_number_statuses read_number(char** number_x, FILE* file_r, int* is_double);

/// <summary>
/// Gets a string number of double type, makes it lowercase, finds number's min notation and converts it in decimal notation
/// </summary>
/// <param name="number_x"> - string number</param>
/// <param name="not"> - min notation of number to be found</param>
/// <param name="number_10"> - number in decimal notation</param>
void fnotation(char* number_x, int* not, long double* number_10);

/// <summary>
/// Gets a string number of int type, makes it lowercase, finds number's min notation and converts it in decimal notation
/// </summary>
/// <param name="number_x"> - string number</param>
/// <param name="not"> - min notation of number to be found</param>
/// <param name="number_10"> - number in decimal notation</param>
void notation(char* number_x, int* not, long* number_10);

/// <summary>
/// Converts number of long type into a string
/// </summary>
/// <param name="number"> - number to convert</param>
/// <returns>NULL if error else string</returns>
const char* my_itoa(long number);

/// <summary>
/// Converts number of long double type into a string. Hidden call of my_itoa
/// </summary>
/// <param name="number"> - number to convert</param>
/// <returns>NULL if error else string</returns>
const char* fmy_itoa(long double number);

#endif // HEADER
