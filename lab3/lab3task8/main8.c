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

	printf("\n\nBinary tree after putting in file out.txt:\n\n");
	print_bin_tree(stdout, bin_tree);
#pragma endregion

	printf("Enter word to check how many times it occured in file %s:\n>>> ", argv[1]);
	char user_input[BUFSIZ];
	scanf("%s", user_input);
	times_met_str times_met_struct;
	find_times_met_statuses find_times_met_s = find_times_met(&bin_tree, user_input, &times_met_struct);
	if (find_times_met_s == find_times_met_no_such_word) {
		printf("No %s in file %s found!\n\n", user_input, argv[1]);
	}
	else {
		printf("Lexeme %s occured in file %s %Iu times\n\n", user_input, argv[1], times_met_struct.count_occured);
	}

	
	unsigned int find_n_most = 0, iter_most_often = 0, processed = 0;
	char** n_most_often_lexemes = NULL;
	printf("Enter wished count of words which are most often met in file %s:\n>>> ", argv[1]);
	scanf("%d", &find_n_most);
	find_n_most_often_statuses find_n_most_often_s = find_n_often(bin_tree, &n_most_often_lexemes,
		&processed, find_n_most);
	switch (find_n_most_often_s)
	{
	case find_n_most_often_incorrect_n_to_find:
		printf("n must not be <= 0!\n");
		break;
	case find_n_most_often_malloc_error:
		printf("Memory allocation error happened!\n");
		delete_binary_tree(&(bin_tree.root));
		return -11;
	case find_n_most_often_count_of_elements_is_less:
		printf("Count of elements in binary tree (%u) is less then your n = %u\n\n", processed, find_n_most);
		break;
	case find_n_most_often_incorrect_ptr_to_lexeme:
		printf("Incorrect ptr to lexemes array passed!\n");
		break;
	default:
		printf("%d most often met lexemes:\n", find_n_most);
		for (iter_most_often = 0; iter_most_often < find_n_most; iter_most_often++) {
			printf("%s ", n_most_often_lexemes[iter_most_often]);
		}
		printf("\n\n");
		free(n_most_often_lexemes);
		break;
	}
	

	// ok omg my string wizard is is is is
	find_min_max_str min_max;
	find_min_max_statuses find_min_max_s = find_min_max(bin_tree, &min_max);
	if (find_min_max_s == find_min_max_tree_is_empty) {
		printf("Your tree is empty! So it's not possible to find min and max lexemes!\n");
	}
	else {
		printf("Max lexeme in your binary tree: %s\nMin lexeme in your binary tree: %s\n\n", 
			min_max.max, min_max.min);
	}

	delete_binary_tree(&(bin_tree.root));
	return 0;
}