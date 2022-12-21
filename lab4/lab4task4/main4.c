#include "header4.h"
#define count_of_arrays 26

void help();
int handle_responses(funcs_statuses funcs_s);
void init_arrays(long_long_array** arrays);
void delete_arrays(long_long_array** arrays);

int main(void)
{
	char user_input[BUFSIZ] = { 0 };
	
	unsigned exited = 0, num_of_array = 0;
	int response = 0;
	funcs_statuses funcs_s = -1;
	long_long_array* arrays = (long_long_array*)malloc(sizeof(long_long_array) * count_of_arrays);
	commands current_command = -1;
	size_t user_input_len = 0;

	if (!arrays) {
		handle_responses(funcs_malloc_error);
		return -1;
	}

	init_arrays(&arrays);

	help();

	while (!exited)
	{
		printf(">>> ");
		fgets(user_input, BUFSIZ, stdin);
		current_command = find_command(user_input, &user_input_len, &num_of_array);

		if (user_input[user_input_len - 1] == '\n') {
			user_input[user_input_len - 1] = '\0';
			user_input_len--;
		}

		switch (current_command)
		{
		case load_command:
			funcs_s = do_load_command(user_input, user_input_len, &(arrays[num_of_array]));
			break;
		case save_command:
			funcs_s = do_save_command(user_input, user_input_len, &(arrays[num_of_array]));
			break;
		case rand_command:
			funcs_s = do_rand_command(user_input, user_input_len, &(arrays[num_of_array]));
			break;
		case concat_command:
			funcs_s = do_concat_command(user_input, user_input_len, arrays);
			break;
		case free_command:
			funcs_s = do_free_command(user_input, user_input_len, arrays);
			break;
		case remove_command:
			funcs_s = do_remove_command(user_input, user_input_len, &(arrays[num_of_array]));
			break;
		case copy_command:
			funcs_s = do_copy_command(user_input, user_input_len, arrays);
			break;
		case sort_command:
			funcs_s = do_remove_command(user_input, user_input_len, &(arrays[num_of_array]));
			break;
		case shuffle_command:
			funcs_s = do_shuffle_command(&(arrays[num_of_array]));
			break;
		case stats_command:
			funcs_s = do_stats_command(&(arrays[num_of_array]));
			break;
		case print_command:
			funcs_s = do_print_command(user_input, user_input_len, &(arrays[num_of_array]));
			break;
		case exit_command:
			exited = 1;
			break;
		default:
			printf("Invalid input! Try again...\n\n");
			break;
		}
		response = handle_responses(funcs_s);
		if (response != 0) {
			delete_arrays(&arrays);
			free(arrays);
			return response;
		}
	}

	delete_arrays(&arrays);
	free(arrays);
	return 0;
}

void help()
{
	printf("The programm operates with array of whole numbers\n");
	printf("Below if the list of commands:\n");
	printf("\tLoad <array_name>, <filename> - load whole numbers in arrayname from filename\n");
	printf("\tSave <array_name>, <filename> - save whole numbers from arrayname to filename\n");
	printf("\tRand <array_name>, count, lb, rb - fill <array_name> with count random numbers from [lb:rb]\n");
	printf("\tConcat <array_name1>, <array_name2> - concatenate arrayname1 and arrayname2.\nResult is saved in first, second is not empty after\n");
	printf("\tFree(<array_name>) - delete <array_name>\n");
	printf("\tRemove <array_name>, <start index>, <end index>\n");
	printf("\tCopy <array_name1>, <start index>, <end index>, <array_name2> - array B must be empty\n");
	printf("\tSort A+ - sort elements of array A in non-descending order\n");
	printf("\tSort A- - sort elements of array A in non-ascending order\n");
	printf("\tShuffle A - rearrange array elements in pseudo-random order\n");
	printf("\tStats A - print statistics about an array\n");
	printf("\tPrint A, 4, 16 - output elements to standard output array starting from 4 to 16\n");
	printf("\tPrint A, all - display all elements of an array\n");
}

int handle_responses(funcs_statuses funcs_s)
{
	switch (funcs_s)
	{
	case funcs_incorrect_ptr_to_array_passed:
		printf("Incorrect pointer to an array passed\n");
		return 0;
	case funcs_incorrect_ptr_to_file_passed:
		printf("Incorrect pointer to file passed\n");
		return -1;
	case funcs_error_index:
		printf("Incorrect index passed\n");
		return 0;
	case funcs_empty_arr:
		printf("Enetered array is empty\n");
		return 0;
	case funcs_malloc_error:
		printf("Memory allocation error happened\n");
		return -2;
	case funcs_realloc_error:
		printf("Memory realloaction error happened\n");
		return -3;
	case funcs_open_file_error:
		printf("Error opening file\n");
		return -4;
	default:
		break;
	}
}

void init_arrays(long_long_array** arrays)
{
	unsigned i;
	for (i = 0; i < count_of_arrays; i++) {
		(*arrays)[i].array = NULL;
		(*arrays)[i].count = 0;
	}
}

void delete_arrays(long_long_array** arrays)
{
	unsigned i;
	for (i = 0; i < count_of_arrays; i++) {
		delete_long_long_array(&((*arrays)[i]));
	}
}