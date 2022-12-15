#include "tree10-1.h"

int main(int argc, char** argv)
{
	if (argc != 3) {
		printf("Incorrect count of command line argumnets\n\n");
		return -1;
	}

	FILE* in = fopen(argv[1], "r"), * out = fopen(argv[2], "w");
	if (!in || !out) {
		printf("Error opening file(s)!\n\n");
	}

	read_n_build_n_print_trees_statuses read_n_build_n_print_trees_s = read_n_build_n_print_trees(in, out);
	switch (read_n_build_n_print_trees_s)
	{
	case read_n_build_trees_incorrect_file_ptr:
		printf("Incorrect file ptr passed to function read_n_build_n_print_trees\n\n");
		return -3;
	case read_n_build_trees_malloc_error:
		printf("Memory allocation error happened!\n\n");
		return -4;
	case read_n_build_trees_realloc_error:
		printf("Memory reallocation error happened!\n\n");
		return -5;
	default:
		printf("The tree(s) were successfully read from file %s and put in file %s\n\n", argv[1], argv[2]);
		break;
	}

	fclose(in);
	fclose(out);

	return 0;
}