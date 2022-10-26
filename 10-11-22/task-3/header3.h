#ifndef HEADER_3

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define SWAP(T, a, b) do { T tmp = a; a = b; b = tmp; } while (0)

typedef struct employee {
	unsigned int id;
	char name[80];
	char surname[80];
	double wage;
} employee;

typedef enum read_employees_statuses {
	read_employees_file_error,
	read_employees_malloc_error,
	read_employees_realloc_error,
	read_employees_ok,
} read_employees_statuses;

typedef struct heapNode {
	employee* key;
	struct heapNode* parent;
	struct heapNode* child;
	struct heapNode* left;
	struct heapNode* right;
	int degree;
} heapNode;

typedef struct fibonacci_heap {
	int size;
	heapNode* root;
} fibonacci_heap;

typedef enum fibonacci_insert_statuses {
	fibonacci_insert_heap_incorrect_heap_ptr,
	fibonacci_insert_malloc_error,
	fibonacci_insert_ok,
} fibonacci_insert_statuses;

typedef enum fibonacci_heap_sort_statuses {
	fibonacci_heap_sort_unexpected_error,
	fibonacci_heap_sort_malloc_error,
	fibonacci_heap_sort_ok
} fibonacci_heap_sort_statuses;

void print_employees(FILE* stream, employee* array, int size);

/// <summary>
/// Reads an array of employees from file, file data must be in format "id" "name" "surname" "salary"\n. If data not valid, the line is skipped.
/// </summary>
/// <param name="fi">- input file</param>
/// <param name="count">- count of read lines</param>
/// <param name="read_employee_s">- exit status</param>
/// <returns>a dynamic array of type of struct employee</returns>
employee* read_employees(FILE* fi, int* count, read_employees_statuses* read_employee_s);

/// <summary>
/// Sortes an array of employees using fibonacci heap and prints sorted array in stream "fi"
/// </summary>
/// <param name="operation">- '>' if root element must be max, the opposite char if root must be min</param>
/// <param name="array">- a read dynamic array of employees</param>
/// <param name="arr_size">- count of elements in employee array</param>
/// <param name="fi">- stream to output the results of sorting</param>
/// <returns>fibonacci_heap_sort_statuses</returns>
fibonacci_heap_sort_statuses fibonacci_heap_sort(char operation, employee* array, int arr_size, FILE* fi);

#endif // !HEADER_3
