#include "header10.h"

int main(int argc, char** argv)
{
	if (argc != 2) {
		printf("Incorrect count of command line arguments!\n");
		return -1;
	}

	unsigned int N = atoi(argv[1]), capable_to_del = N / 2;

	if (N <= 1) {
		printf("Incorrect N! N must be a whole value > 1!\n");
		return -2;
	}

	// service words: cancel, stop
	FILE* out = fopen("out.txt", "w");
	if (!out) {
		printf("Error opening resulting file!\n");
		return -3;
	}

	read_str_with_special_words_statuses read_response = -1;
	signly_linked_list_node* tmp = NULL;
	char* string = NULL;
	int cancel_response = 0;

	stack st;
	queue qu;
	st.top_element = NULL;
	qu.first_element = NULL;
	qu.last_element = NULL;
	qu.nodes_count = 0;

	while ((read_response = read_str_with_special_words(NULL, &string)) != read_str_with_special_words_stop) {
		switch (read_response)
		{
		case read_str_with_special_words_cancel:
			if ((cancel_response = cancel(&st, &qu, capable_to_del)) != 0) {
				printf("There are no enough strings to delete from your list!\nYou should write %d more\n", cancel_response);
				continue;
			}

			printf("Nodes were successfully deleted from your list!\n");
			break;
		case read_str_with_special_words_malloc_error:
			printf("Memory allocation error happened!\n");
			fclose(out);
			return -4;
		case read_str_with_special_words_realloc_error:
			printf("Memory reallocation error happened!\n");
			fclose(out);
			return -5;
		default:
			break;
		}

		if (read_response != read_str_with_special_words_cancel) {
			if (insert_string(&st, &qu, string) != 0) {
				printf("Memory allocation/reallocation error happened!\n");
				fclose(out);
				return -6;
			}

			if (qu.nodes_count == N + 1) {
				put_in_file(out, &st, &qu, N);
			}
		}
	}

	put_in_file(out, &st, &qu, qu.nodes_count);

	delete_stack_n_queue(&st, &qu);
	fclose(out);
	return 0;
}