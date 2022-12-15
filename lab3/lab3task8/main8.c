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

#pragma region putting in file and recoverying from file
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

	printf("Binary tree read from file %s before putting in file out.txt:\n", argv[1]);
	print_bin_tree(stdout, bin_tree);

	if (!(out = fopen("out.txt", "w"))) {
		printf("Error opening file\n");
		return -6;
	}
	put_in_file_statuses put_in_file_s = put_in_file(out, bin_tree);
	fclose(out);
	delete_binary_tree(&(bin_tree.root));
	bin_tree.root = NULL;

	if (!(in = fopen("out.txt", "r"))) {
		printf("Error opening file\n");
		return -7;
	}

	read_in_binary_tree_s = read_in_binary_tree(in, &bin_tree);
	fclose(in);

	if (read_in_binary_tree_s != read_in_binary_tree_ok) {
		switch (read_in_binary_tree_s)
		{
		case read_in_binary_tree_incorrect_ptr_to_file:
			printf("Incorrect ptr to file passed in fuction read_in_binary_tree\n");
			return -8;
		case read_in_binary_tree_malloc_error:
			printf("Memory allocation error happened!\n");
			return -9;
		case read_in_binary_tree_realloc_error:
			printf("Memory reallocation error happened!\n");
			return -10;
		default:
			break;
		}
	}

	printf("\n\nBinary tree after putting in file out.txt:\n");
	print_bin_tree(stdout, bin_tree);
#pragma endregion

	delete_binary_tree(&(bin_tree.root));
	return 0;
}