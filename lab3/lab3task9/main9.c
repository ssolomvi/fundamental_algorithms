#include "header9.h"
#include "dbl_list.h"
#include "bin_heap.h"

int main(int argc, char** argv)
{
	if (argc == 1 || argc % 2 == 0) {
		printf("Incorrect count of command line arguments\n");
		return -1;
	}

	container* input_arguments = NULL;
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
		printf("All data was successfully read from command line!\n");
		break;
	}

	dbl_list my_dbl_list;
	my_dbl_list.head = NULL;

	read_from_file_dbl_list_statuses read_from_file_dbl_s = read_from_file_dbl_list(&my_dbl_list, input_arguments, count_of_files);
	switch (read_from_file_dbl_s)
	{
	case read_from_file_dbl_list_error_opening_file:
		printf("Incorrect filename or incorrect command line arguments passed\n");
		return -6;
	case read_from_file_dbl_list_fseek_error:
		printf("Fseek error happened!\n");
		return -7;
	case read_from_file_dbl_list_malloc_error:
		printf("Memory allocation error happened!\n");
		return -8;
	default:
		printf("All data was successfully put in doubly-linked list\n\n");
		break;
	}
	extract_n_print(stdout, &my_dbl_list);

	free_container_arr(&input_arguments, &count_of_files);
	return 0;
}
