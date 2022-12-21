#include "header4.h"
#include "array4.h"

char to_lower(char ch)
{
	return ((ch >= 'A' && ch <= 'Z') ? ch + 32 : ch);
}

int is_letter(char ch)
{
	return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'));
}

unsigned determine_array_index(char ch)
{
	return (to_lower(ch) - 'a');
}

commands find_command(char* str, size_t* length, unsigned* num_of_array)
{
	char ar = 0, comm[8] = { 0 };
	(*length) = strlen(str);
	unsigned i;
	for (i = 0; i < (*length) && i < 8; i++) {
		if (is_letter(str[i])) {
			comm[i] = to_lower(str[i]);
		}
		else
			break;
	}
	comm[i] = '\0';

	if ((*length) == 4 && !strcmp(comm, "exit"))
		return exit_command;

	if ((*length) <= i + 1)
		return not_a_command;

	ar = str[i + 1];
	if (!(is_letter))
		return not_a_command;

	(*num_of_array) = determine_array_index(ar);
	
	if (!strcmp(comm, "load")) {
		return load_command;
	}
	if (!strcmp(comm, "save")) {
		return save_command;
	}
	if (!strcmp(comm, "rand")) {
		return rand_command;
	}
	if (!strcmp(comm, "concat")) {
		return concat_command;
	}
	if (!strcmp(comm, "free")) {
		return free_command;
	}
	if (!strcmp(comm, "remove")) {
		return remove_command;
	}
	if (!strcmp(comm, "copy")) {
		return copy_command;
	}
	if (!strcmp(comm, "sort")) {
		return sort_command;
	}
	if (!strcmp(comm, "shuffle")) {
		return shuffle_command;
	}
	if (!strcmp(comm, "stats")) {
		return stats_command;
	}
	if (!strcmp(comm, "print")) {
		return print_command;
	}

	return not_a_command;
}

funcs_statuses do_load_command(char* input, size_t len, long_long_array* array)
{
	char filename[FILENAME_MAX] = { 0 }, tmp = 0;
	if (len <= 8)
		return funcs_command_error;

	size_t i, j;
	for (i = 8, j = 0; i < len; j++, i++) {
		filename[j] = input[i];
	}
	filename[j] = '\0';

	FILE* in = NULL;
	if (!(in = fopen(filename, "r")))
		return funcs_open_file_error;

	funcs_statuses funcs_s = load(in, array);
	fclose(in);
	return funcs_s;
}

funcs_statuses do_save_command(char* input, size_t len, long_long_array* array)
{
	char filename[FILENAME_MAX] = { 0 }, tmp = 0;
	if (len <= 8)
		return funcs_command_error;

	size_t i, j;
	for (i = 8, j = 0; i < len; j++, i++) {
		filename[j] = input[i];
	}
	filename[j] = '\0';

	FILE* out = NULL;
	if (!(out = fopen(filename, "w")))
		return funcs_open_file_error;

	funcs_statuses funcs_s = save(out, array);
	fclose(out);
	return funcs_s;
}

funcs_statuses do_rand_command(char* input, size_t len, long_long_array* array)
{
	size_t count = 0;
	int lb = 0, rb = 0;

	if ((sscanf(input, "%Iu, %d, %d", &count, &lb, &rb)) != 3)
		return funcs_command_error;

	funcs_statuses funcs_s = load_rand(array, count, lb, rb);
	return funcs_s;
}

funcs_statuses do_concat_command(char* input, size_t len, long_long_array* arrays)
{
	char first = 0, second = 0, * ptr = input + 7;
	if ((len <= 9))
		return funcs_command_error;
	if (!is_letter((first = input[7])) || !is_letter((second = input[9])))
		return funcs_command_error;

	funcs_statuses funcs_s = concat(&(arrays[determine_array_index(first)]), &(arrays[determine_array_index(second)]));
	return funcs_s;
}

funcs_statuses do_free_command(char* input, size_t len, long_long_array* arrays)
{
	char ar = 0;
	if (len == 7 && to_lower(input[0]) == 'f' && to_lower(input[1]) == 'r' && to_lower(input[2]) == 'e'
		&& to_lower(input[3]) == 'e') {
		if (!is_letter((ar = input[5])))
			return funcs_command_error;
		delete_long_long_array(&(arrays[determine_array_index(ar)]));
	}
	else
		return funcs_command_error;
	return funcs_ok;
}

funcs_statuses do_remove_command(char* input, size_t len, long_long_array* array)
{
	size_t start = 0, end = 0;
	if ((sscanf(input, "%Iu, %Iu", &start, &end)) != 2)
		return funcs_command_error;

	funcs_statuses funcs_s = remove_from_arr(array, start, end);
	return funcs_s;
}

funcs_statuses do_copy_command(char* input, size_t len, long_long_array* arrays)
{
	size_t start = 0, end = 0;
	char ar1 = 0, ar2 = 0, * tmp = input + 7;

	if (len <= 12)
		return funcs_command_error;

	ar1 = input[5];

	if ((sscanf(tmp, "%Iu, %Iu, %c", &start, &end, &ar2)) != 3)
		return funcs_command_error;

	if (!is_letter(ar1) || !is_letter(ar2))
		return funcs_command_error;

	funcs_statuses funcs_s = copy(&(arrays[determine_array_index(ar1)]), start, end, &(arrays[determine_array_index(ar2)]));
	return funcs_s;
}

funcs_statuses do_sort_command(char* input, size_t len, long_long_array* array)
{
	if (len != 7)
		return funcs_command_error;

	char ch = input[6];
	if (ch == '+')
		sort(array, compare_non_decreasing);
	else if (ch == '-')
		sort(array, compare_non_increasing);
	else
		return funcs_command_error;
	return funcs_ok;
}

funcs_statuses do_shuffle_command(long_long_array* array)
{
	shuffle(array);
}

funcs_statuses do_stats_command(long_long_array* array)
{
	if (stats(stdout, array) != 0)
		return funcs_malloc_error;
	return funcs_ok;
}

funcs_statuses do_print_command(char* input, size_t len, long_long_array* array)
{
	size_t start = 0, end = 0;

	if ((sscanf(input, "%Iu, %Iu", &start, &end)) != 2) {
		if (len <= 12)
			return funcs_command_error;
		if (to_lower(input[9]) == 'a' && to_lower(input[10] == 'l') && to_lower(input[11]) == 'l') {
			start = 0;
			end = array->count - 1;
		}
		else {
			return funcs_command_error;
		}
	}
	funcs_statuses funcs_s = print_from_to(stdout, *array, start, end);
	return funcs_s;
}
