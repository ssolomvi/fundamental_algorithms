#define _CRT_SECURE_NO_WARNINGS

#ifndef DBL_LIST
#pragma once
#include <string.h>
#include "header9.h"

typedef struct dbl_elem {
	T* data;
	struct dbl_elem* next;
	struct dbl_elem* prev;
} dbl_elem;

typedef struct dbl_list {
	dbl_elem* head;
} dbl_list;

typedef enum read_from_file_dbl_list_statuses {
	read_from_file_dbl_list_malloc_error,
	read_from_file_dbl_list_error_opening_file,
	read_from_file_dbl_list_fseek_error,
	read_from_file_dbl_list_ok
} read_from_file_dbl_list_statuses;

read_from_file_dbl_list_statuses read_from_file_dbl_list(dbl_list* list, container* input_arguments, size_t count_of_files);
void extract_n_print_dbl_list(FILE* stream, dbl_list* list);

# endif // DBL_LIST
