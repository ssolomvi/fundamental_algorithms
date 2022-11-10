#include "header8.h"

int main(int argc, char** argv)
{
	if (argc != 2) {
		printf("Count of command line is incorrect\n");
		return -1;
	}

	FILE* in = NULL, * out = NULL;
	if (!(in = fopen(argv[1], "r"))) {
		printf("Error opening file\n");
		return -2;
	}

	binary_tree bin_tree;
	bin_tree.root = NULL;

	read_in_binary_tree_statuses read_in_binary_tree_s = read_in_binary_tree(in, &bin_tree);
	fclose(in);

	if (read_in_binary_tree_s != read_in_binary_tree_ok) {
		switch (read_in_binary_tree_s)
		{
		case read_in_binary_tree_incorrect_ptr_to_file:
			printf("Incorrect ptr to file passed in fuction read_in_binary_tree\n");
			return -3;
		case read_in_binary_tree_malloc_error:
			printf("Memory allocation error happened!\n");
			return -4;
		case read_in_binary_tree_realloc_error:
			printf("Memory reallocation error happened!\n");
			return -5;
		default:
			break;
		}
	}


	delete_binary_tree(&(bin_tree.root));
	return 0;
}