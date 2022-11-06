#ifndef STRING_6
#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

typedef struct string {
	char* str;
	size_t length;
} string;

/// <summary>
/// Finds length of a str (char* type). String must be null-terminated
/// </summary>
/// <param name="str"></param>
/// <returns>size of str</returns>
size_t length_string(char* str);

/// <summary>
/// Prints a struct string by character in entered stream
/// </summary>
/// <param name="stream">- stream where to print</param>
/// <param name="str">- string to print</param>
void print_string(FILE* stream, string str);

/// <summary>
/// Makes a new struct string object
/// </summary>
/// <param name="stream">- stream from where to read new string struct properties</param>
/// <param name="to_allocate">- 0 if unknown</param>
/// <param name="divider">- char which divides string from another data, e.g. '\n'</param>
/// <returns>new string</returns>
string create_string(FILE* stream, size_t to_allocate, char divider);

/// <summary>
/// If memory for dynamic array in struct string allocated, cleans it, changes the length
/// </summary>
/// <param name="str">- string to delete</param>
void delete_string(string* str);

/// <summary>
/// Compares string with some comparator function. If strings lengths are 0, returns 0
/// </summary>
/// <param name="first">- string struct 1 to compare</param>
/// <param name="second">- string struct 2 to compare</param>
/// <param name="compare">- a function which compares two strings, e.g. lexicographic comparator</param>
/// <returns>value less 0 if first is less than second; value == 0 if first == second, value bigger 0 if first is bigger than second</returns>
int compare_strings(string first, string second, int compare(string, string));

/// <summary>
/// Copies struct string original values to strict string copy. Copy data is freed
/// </summary>
/// <param name="copy">- where to copy the data</param>
/// <param name="original">- copied string</param>
void copy_string(string* copy, string original);

/// <summary>
/// Concatenates strings (appends one to another)
/// </summary>
/// <param name="num_strings">- count of strings to concatenate</param>
/// <param name="">- string to concatenate</param>
/// <returns>Struct string -- a result of concatenation</returns>
string cat_strings(int num_strings, ...);

/// <summary>
/// Makes a new string which is a double of struct string original
/// </summary>
/// <param name="original">- string to double</param>
/// <returns>a new string object</returns>
string double_string(string original);

int lexic_comparator(string first, string second);

#endif // !STRING_6
