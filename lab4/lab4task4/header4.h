#define _CRT_SECURE_NO_WARNINGS

#ifndef HEADER_4
#pragma once

#include "array4.h"

typedef enum commands {
	load_command,
	save_command,
	rand_command,
	concat_command,
	free_command,
	remove_command,
	copy_command,
	sort_command,
	shuffle_command,
	stats_command,
	print_command,
	exit_command,
	not_a_command,
} commands;

commands find_command(char* str, size_t* length, unsigned* num_of_array);

funcs_statuses do_load_command(char* input, size_t len, long_long_array* array);
funcs_statuses do_save_command(char* input, size_t len, long_long_array* array);
funcs_statuses do_rand_command(char* input, size_t len, long_long_array* array);
funcs_statuses do_concat_command(char* input, size_t len, long_long_array* arrays);
funcs_statuses do_free_command(char* input, size_t len, long_long_array* arrays);
funcs_statuses do_remove_command(char* input, size_t len, long_long_array* array);
funcs_statuses do_copy_command(char* input, size_t len, long_long_array* arrays);
funcs_statuses do_sort_command(char* input, size_t len, long_long_array* array);
funcs_statuses do_shuffle_command(long_long_array* array);
funcs_statuses do_stats_command(long_long_array* array);
funcs_statuses do_print_command(char* input, size_t len, long_long_array* array);

#endif // !HEADER_4
