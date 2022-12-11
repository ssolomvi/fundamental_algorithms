#include "header9.h"

void free_container_arr(container** arr, size_t* allocated)
{
	size_t i;
	for (i = 0; i < (*allocated); i++) {
		delete_string(&((*arr)[i].data.text));
	}
	free(*arr);
	(*arr) = NULL;
	(*allocated) = 0;
}

void free_T_arr(T** arr, size_t* allocated)
{
	size_t i;
	for (i = 0; i < (*allocated); i++) {
		delete_string(&(*arr)[i].text);
	}
	free(*arr);
	(*arr) = NULL;
	(*allocated) = 0;
}

read_argv_statuses read_argv(int argc, char** argv, container** arr, size_t* allocated)
{
	if (*arr) {
		return read_argv_incorrect_arr_ptr;
	}

	int count_of_files = ((argc - 1) / 2);
	if (!((*arr) = (container*)malloc(sizeof(container) * count_of_files))) {
		return read_argv_malloc_error;
	}
	(*allocated) = 0;

	size_t i, j, k, argv_len;
	for (i = 0, j = 1; i < count_of_files; i++, j += 2) {
		if (sscanf(argv[j], "prior=%d", &(*arr)[i].data.priority) != 1) {
			free_container_arr(arr, allocated);
			return read_argv_error_readind_data_prior;
		}

		if ((*arr)[i].data.priority < 0 || (*arr)[i].data.priority > 10) {
			free_container_arr(arr, allocated);
			return read_argv_error_readind_data_prior;
		}

		argv_len = strlen(argv[j + 1]) - 7 + 1;

		if (argv_len <= 1) {
			free_container_arr(arr, allocated);
			return read_argv_error_readind_data_text;
		}

		if (!((*arr)[i].data.text.str = (char*)malloc(sizeof(char) * argv_len))) {
			free_container_arr(arr, allocated);
			return read_argv_malloc_error;
		}

		for (k = 0; k < argv_len - 1; k++) {
			(*arr)[i].data.text.str[k] = argv[j + 1][6 + k];
		}
		(*arr)[i].data.text.str[argv_len - 1] = '\0';
		(*arr)[i].data.text.length = argv_len - 1;
		(*arr)[i].bytes_read = 0;
		(*arr)[i].is_read = 0;
		(*allocated)++;
	}
	return read_argv_ok;
}

/*
read_argv_statuses read_argv(int argc, char** argv, T** arr, size_t* allocated)
{
	if (*arr) {
		return read_argv_incorrect_arr_ptr;
	}

	int count_of_files = ((argc - 1) / 2);
	if (!((*arr) = (T*)malloc(sizeof(T) * count_of_files))) {
		return read_argv_malloc_error;
	}
	(*allocated) = 0;

	size_t i, j, k, argv_len;
	for (i = 0, j = 1; i < count_of_files; i++, j += 2) {
		if (sscanf(argv[j], "prior=%d", &(*arr)[i].priority) != 1) {
			free_T_arr(arr, allocated);
			return read_argv_error_readind_data_prior;
		}

		if ((*arr)[i].priority < 0 || (*arr)[i].priority > 10) {
			free_T_arr(arr, allocated);
			return read_argv_error_readind_data_prior;
		}

		argv_len = strlen(argv[j + 1]) - 7 + 1;

		if (argv_len <= 1) {
			free_T_arr(arr, allocated);
			return read_argv_error_readind_data_text;
		}

		if (!((*arr)[i].text.str = (char*)malloc(sizeof(char) * argv_len))) {
			free_T_arr(arr, allocated);
			return read_argv_malloc_error;
		}

		for (k = 0; k < argv_len - 1; k++) {
			(*arr)[i].text.str[k] = argv[j + 1][6 + k];
		}
		(*arr)[i].text.str[argv_len - 1] = '\0';
		(*arr)[i].text.length = argv_len - 1;
		(*allocated)++;
	}
	return read_argv_ok;
}
*/
