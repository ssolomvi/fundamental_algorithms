#define _CRT_SECURE_NO_WARNINGS

#ifndef ARRAY_4
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef enum funcs_statuses {
	funcs_incorrect_ptr_to_array_passed,
	funcs_open_file_error,
	funcs_command_error,
	funcs_incorrect_ptr_to_file_passed,
	funcs_error_index,
	funcs_empty_arr,
	funcs_malloc_error,
	funcs_realloc_error,
	funcs_ok
} funcs_statuses;

typedef struct long_long_array {
	long long* array;
	size_t count;
} long_long_array;

funcs_statuses load(FILE* in, long_long_array* arr);
funcs_statuses save(FILE* out, long_long_array* arr);
funcs_statuses load_rand(long_long_array* arr, size_t count, int lb, int lr);
funcs_statuses concat(long_long_array* A, long_long_array* B);
void delete_long_long_array(long_long_array* arr);
funcs_statuses remove_from_arr(long_long_array* arr, size_t start_index, size_t count_to_del);
funcs_statuses copy(long_long_array* A, size_t start_ind, size_t end_ind, long_long_array* B);
void sort(long_long_array* arr, int funct(void*, void*));
int compare_non_decreasing(void* first, void* second);
int compare_non_increasing(void* first, void* second);
void shuffle(long_long_array* arr);
int stats(FILE* stream, long_long_array* arr);
funcs_statuses print_from_to(FILE* stream, long_long_array arr, size_t start_ind, size_t end_ind);

#endif // !ARRAY_4
