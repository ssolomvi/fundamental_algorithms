#include "header4.h"

int main(int argc, char** argv)
{
	if (argc != 2) {
		printf("Incorrect count of command line arguments.\nPass the stop lexeme as command line argument\n");
		return -1;
	}

	int tries = 3;
	char user_stream[BUFSIZ], ** messages = NULL, * filename_out = gen_file_name();
	FILE* out = NULL, * in = NULL;
	if (!(out = fopen(filename_out, "w"))) {
		printf("Error opening output scv file\n");
		return -2;
	}
	read_n_write_csv_statuses read_n_write_csv_s = -1;

	while (tries) {
		printf("Enter the source from where to read messages:\n>>> ");

		if ((scanf("%s", user_stream)) != 1) {
			printf("Error reading a string!\n");
			return -3;
		}
		if (!strcmp(user_stream, "stdin")) {
			read_n_write_csv_s = read_n_write_csv(stdin, out, argv[1]);
		}
		else {
			if (!(in = fopen(user_stream, "r"))) {
				tries--;
				printf("Incorrect file name passed / error opening file %s. You have %d tries left\n\n", user_stream, tries);
			}
			read_n_write_csv_s = read_n_write_csv(in, out, argv[1]);
		}

		if (read_n_write_csv_s != -1) {
			switch (read_n_write_csv_s)
			{
			case read_n_write_malloc_error:
				printf("Memory allocation error happened\n");
				return -4;
			case read_n_write_realloc_error:
				printf("Memory reallocation error happened\n");
				return -5;
			case read_n_write_file_ptr_error:
				printf("Incorrect ptr to file passed in function read_n_write_csv\n");
				return -6;
			case read_n_write_incorrect_stop_lexeme:
				printf("Incorrect ptr to stop lexeme / incorrect stop lexeme passed in function read_n_write_csv\n");
				return -7;
			case read_n_write_fgets_error:
				printf("Error happened with fgets function in function read_n_write_csv\n");
				return -8;
			case read_n_write_message_error:
				printf("Incorrect message passed in function in_csv_file\n");
				return -9;
			default:
				printf("All the data was successfully read from input stream!\nIt's wrote down in file %s\n\n", filename_out);
				break;
			}
			break;
		}
	}

	fclose(in);
	fclose(out);

	if (!(out = fopen(filename_out, "r"))) {
		printf("Error opening file\n");
		return -2;
	}
	
	message* arr = NULL;
	int arr_count = 0;
	read_from_csv_file_statuses read_from_csv_file_s = read_from_csv_file(out, &arr, &arr_count);

	switch (read_from_csv_file_s)
	{
	case read_from_csv_file_incorrect_ptr_to_file:
		printf("Incorrect ptr to file passed in function read_from_csv_file\n");
		return -10;
	case read_from_csv_file_incorrect_ptr_to_messages_arr:
		printf("Incorrect ptr to messages array passed in function read_from_csv_file (it must be NULL)\n");
		return -11;
	case read_from_csv_file_malloc_error:
		printf("Memory allocation error happened\n");
		return -12;
	case read_from_csv_file_realloc_error:
		printf("Memory reallocation error happened\n");
		return -13;
	default:

		while (tries) {
			printf("Where do you want a dynamic array of struct message be printed?\n>>> ");
			
			if (scanf("%s", user_stream) != 1) {
				clear(&arr, &arr_count);
				printf("Error reading output stream\n");
				return -14;
			}

			if (!(strcmp(user_stream, "stdout"))) {
				print_message_arr(stdout, arr, arr_count);
				clear(&arr, &arr_count);
				break;
			}
			else {
				if (!(out = fopen(user_stream, "w"))) {
					tries--;
					printf("Error opening file. You have %d tries left", tries);
					continue;
				}
				print_message_arr(out, arr, arr_count);
				clear(&arr, &arr_count);
				break;
			}
		}
		
		break;
	}
	fclose(out);
	free(filename_out);
	return 0;
}