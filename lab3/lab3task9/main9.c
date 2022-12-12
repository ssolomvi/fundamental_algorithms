#include "header9.h"
#include "dbl_list.h"
#include "bin_heap.h"
#include <time.h>
#include <math.h>

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

# pragma region reading from file to doubly linked list

	clock_t start_dbl_list = clock(), end_dbl_list;

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
	extract_n_print_dbl_list(stdout, &my_dbl_list);
	end_dbl_list = clock();
	float total_dbl_list = (double)(end_dbl_list - start_dbl_list) / CLOCKS_PER_SEC;
	printf("The elapsed time is %f seconds for realization using doubly-linked list\n\n", total_dbl_list);
# pragma endregion 

	size_t i;
	for (i = 0; i < count_of_files; i++) {
		input_arguments[i].bytes_read = 0;
		input_arguments[i].is_read = 0;
	}

	bin_heap my_bin_heap;
	my_bin_heap.allocated = 0; my_bin_heap.size = 0; my_bin_heap.heap = NULL;

# pragma region reading from file to binary heap

	clock_t start_bin_heap = clock(), end_bin_heap;

	read_from_file_bin_heap_statuses read_from_file_bin_heap_s = read_from_file_bin_heap(&my_bin_heap, input_arguments, count_of_files);
	switch (read_from_file_bin_heap_s)
	{
	case read_from_file_bin_heap_malloc_error:
		printf("Memory allocation error happened!\n");
		return -9;
	case read_from_file_bin_heap_realloc_error:
		printf("Memory reallocation error happened!\n");
		return -10;
	case read_from_file_bin_heap_open_file_error:
		printf("Incorrect filename or incorrect command line arguments passed\n");
		return -11;
	case read_from_file_bin_heap_fseek_error:
		printf("Fseek error happened!\n");
		return -12;
	default:
		printf("All data was successfully put in binary heap\n\n");
		break;
	}
	extract_n_print_bin_heap(stdout, &my_bin_heap);
	end_bin_heap = clock();
	float total_bin_heap = (double)(end_bin_heap - start_bin_heap) / CLOCKS_PER_SEC;

	printf("The elapsed time is %f seconds for realization using binary heap\n\n", total_bin_heap);
# pragma endregion
	printf("%s realization is faster by %f seconds\n\n",
		total_dbl_list < total_bin_heap ? "Doubly-linked list" : "Binary heap", fabs(total_bin_heap - total_dbl_list));
	free_container_arr(&input_arguments, &count_of_files);
	return 0;
}
