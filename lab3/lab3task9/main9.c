#include "header9.h"

// prior=0 task='Task 1'
// prior=0 task='task_1' prior=2 task='task_3' prior=5 task='task_2'

typedef enum read_argv_statuses {
	read_argv_error_readind_data_prior,
	read_argv_error_readind_data_text,
	read_argv_malloc_error,
	read_argv_incorrect_arr_ptr,
	read_argv_ok
} read_argv_statuses;

read_argv_statuses read_argv(int argc, char** argv, T** arr, size_t* allocated);

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

int main(int argc, char** argv)
{
	if (argc == 1 || argc % 2 == 0) {
		printf("Incorrect count of command line arguments\n");
		return -1;
	}

	T* input_arguments = NULL;
	size_t count_of_files = 0;

	read_argv_statuses read_argv_s = read_argv(argc, argv, &input_arguments, &count_of_files);
	switch (read_argv_s)
	{
	case read_argv_error_readind_data_prior:
		printf("Incorrect prior done\n");
		return -2;
	case read_argv_error_readind_data_text:
		printf("Incorrect filename done\n");
		return -3;
	case read_argv_incorrect_arr_ptr:
		printf("Incorrect ptr to array of T passed. It must be NULL\n");
		return -4;
	case read_argv_malloc_error:
		printf("Memory allocation error happened!\n");
		return -5;
	default:
		printf("The data was successfully read from command line!\n");
		break;
	}

	free_T_arr(&input_arguments, &count_of_files);
	return 0;
}



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
