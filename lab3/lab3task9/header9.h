#ifndef HEADER_9
#define _CRT_SECURE_NO_WARNINGS

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "string6.h"

typedef struct data {
	string text;
	int priority;
} T;

typedef struct container {
	T data;
	long bytes_read;
	int is_read;
} container;

typedef enum read_argv_statuses {
	read_argv_error_readind_data_prior,
	read_argv_error_readind_data_text,
	read_argv_malloc_error,
	read_argv_incorrect_arr_ptr,
	read_argv_ok
} read_argv_statuses;

read_argv_statuses read_argv(int argc, char** argv, container** arr, size_t* allocated);
void free_container_arr(container** arr, size_t* allocated);


#endif // !HEADER_9
