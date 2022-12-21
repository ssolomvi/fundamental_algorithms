#include "header4.h"
#define count_of_arrays 26

void help();
int handle_responses(funcs_statuses funcs_s);
void init_arrays(long_long_array** arrays);
void delete_arrays(long_long_array** arrays);

typedef enum commands {
	load_command,
	save_command,
	rand_command,
	concat_command,
	free_command,
	remove_command,
	copy_command,
	sort_command,
	shuffle_command,
	stats_command,
	print_command,
	exit_command,
	not_a_command,
} commands;

commands find_command(char* str, size_t* length, unsigned* num_of_array);

funcs_statuses do_load_command(char* input, size_t len, long_long_array* array);
funcs_statuses do_save_command(char* input, size_t len, long_long_array* array);
funcs_statuses do_rand_command(char* input, size_t len, long_long_array* array);
funcs_statuses do_concat_command(char* input, size_t len, long_long_array* arrays);
funcs_statuses do_free_command(char* input, size_t len, long_long_array* arrays);
funcs_statuses do_remove_command(char* input, size_t len, long_long_array* array);
funcs_statuses do_copy_command(char* input, size_t len, long_long_array* arrays);
funcs_statuses do_sort_command(char* input, size_t len, long_long_array* array);
funcs_statuses do_shuffle_command(long_long_array* array);
funcs_statuses do_stats_command(long_long_array* array);
funcs_statuses do_print_command(char* input, size_t len, long_long_array* array);

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
			funcs_s = do_sort_command(user_input, user_input_len, &(arrays[num_of_array]));
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
			continue;
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
// load A, 2.txt
// rand B, 100, -20, 800
// load C, 3.txt
// print B, all

void help()
{
	printf("The programm operates with array of whole numbers\n");
	printf("Below if the list of commands:\n");
	printf("\tLoad <array_name>, <filename> - load whole numbers in arrayname from filename\n");
	printf("\tSave <array_name>, <filename> - save whole numbers from arrayname to filename\n");
	printf("\tRand <array_name>, count, lb, rb - fill <array_name> with count random numbers from [lb:rb]\n\n");
	printf("\tConcat <array_name1>, <array_name2> - concatenate arrayname1 and arrayname2.\n\t\tResult is saved in first, second is not empty after\n");
	printf("\tFree(<array_name>) - delete <array_name>\n\n");
	printf("\tRemove <array_name>, <start index>, <end index>\n");
	printf("\tCopy <array_name1>, <start index>, <end index>, <array_name2> - array_name2 shufflmust be empty\n");
	printf("\tSort A+ - sort elements of array A in non-descending order\n\n");
	printf("\tSort A- - sort elements of array A in non-ascending order\n");
	printf("\tShuffle A - rearrange array elements in pseudo-random order\n");
	printf("\tStats A - print statistics about an array\n\n");
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
		return 0;
	case funcs_command_error:
		printf("Incorrect command passed!\n");
		return 0;
	default:
		printf("Done successfully\n");
		return 0;
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

char to_lower(char ch)
{
	return ((ch >= 'A' && ch <= 'Z') ? ch + 32 : ch);
}

int is_letter(char ch)
{
	return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'));
}

unsigned determine_array_index(char ch)
{
	return (to_lower(ch) - 'a');
}

commands find_command(char* str, size_t* length, unsigned* num_of_array)
{
	char ar = 0, comm[8] = { 0 };
	(*length) = strlen(str);
	unsigned i;
	for (i = 0; i < (*length) && i < 8; i++) {
		if (is_letter(str[i])) {
			comm[i] = to_lower(str[i]);
		}
		else
			break;
	}

	if (!strcmp(comm, "exit"))
		return exit_command;

	if ((*length) <= i + 1)
		return not_a_command;

	ar = str[i + 1];
	if (!(is_letter(ar)))
		return not_a_command;

	(*num_of_array) = determine_array_index(ar);

	if (!strcmp(comm, "load")) {
		return load_command;
	}
	if (!strcmp(comm, "save")) {
		return save_command;
	}
	if (!strcmp(comm, "rand")) {
		return rand_command;
	}
	if (!strcmp(comm, "concat")) {
		return concat_command;
	}
	if (!strcmp(comm, "free")) {
		return free_command;
	}
	if (!strcmp(comm, "remove")) {
		return remove_command;
	}
	if (!strcmp(comm, "copy")) {
		return copy_command;
	}
	if (!strcmp(comm, "sort")) {
		return sort_command;
	}
	if (!strcmp(comm, "shuffle")) {
		return shuffle_command;
	}
	if (!strcmp(comm, "stats")) {
		return stats_command;
	}
	if (!strcmp(comm, "print")) {
		return print_command;
	}

	return not_a_command;
}

funcs_statuses do_load_command(char* input, size_t len, long_long_array* array)
{
	char filename[FILENAME_MAX] = { 0 }, tmp = 0;
	if (len <= 8)
		return funcs_command_error;

	size_t i, j;
	for (i = 8, j = 0; i < len; j++, i++) {
		filename[j] = input[i];
	}

	FILE* in = NULL;
	if (!(in = fopen(filename, "r")))
		return funcs_open_file_error;

	funcs_statuses funcs_s = load(in, array);
	fclose(in);
	return funcs_s;
}

funcs_statuses do_save_command(char* input, size_t len, long_long_array* array)
{
	char filename[FILENAME_MAX] = { 0 }, tmp = 0;
	if (len <= 8)
		return funcs_command_error;

	size_t i, j;
	for (i = 8, j = 0; i < len; j++, i++) {
		filename[j] = input[i];
	}

	FILE* out = NULL;
	if (!(out = fopen(filename, "w")))
		return funcs_open_file_error;

	funcs_statuses funcs_s = save(out, array);
	fclose(out);
	return funcs_s;
}

funcs_statuses do_rand_command(char* input, size_t len, long_long_array* array)
{
	size_t count = 0;
	int lb = 0, rb = 0;
	char* ptr = input + 7;

	if ((sscanf(ptr, "%Iu, %d, %d", &count, &lb, &rb)) != 3)
		return funcs_command_error;

	funcs_statuses funcs_s = load_rand(array, count, lb, rb);
	return funcs_s;
}

funcs_statuses do_concat_command(char* input, size_t len, long_long_array* arrays)
{
	char first = 0, second = 0;
	if ((len <= 10))
		return funcs_command_error;
	if (!is_letter((first = input[7])) || !is_letter((second = input[10])))
		return funcs_command_error;
	unsigned first_arr = determine_array_index(first), second_arr = determine_array_index(second);

	funcs_statuses funcs_s = concat(&(arrays[first_arr]), &(arrays[second_arr]));
	return funcs_s;
}

funcs_statuses do_free_command(char* input, size_t len, long_long_array* arrays)
{
	char ar = 0;
	if (len == 7 && to_lower(input[0]) == 'f' && to_lower(input[1]) == 'r' && to_lower(input[2]) == 'e'
		&& to_lower(input[3]) == 'e') {
		if (!is_letter((ar = input[5])))
			return funcs_command_error;
		delete_long_long_array(&(arrays[determine_array_index(ar)]));
	}
	else
		return funcs_command_error;
	return funcs_ok;
}

funcs_statuses do_remove_command(char* input, size_t len, long_long_array* array)
{
	size_t start = 0, end = 0;
	char* ptr = input + 9;
	if ((sscanf(ptr, "%Iu, %Iu", &start, &end)) != 2)
		return funcs_command_error;

	funcs_statuses funcs_s = remove_from_arr(array, start, end);
	return funcs_s;
}

funcs_statuses do_copy_command(char* input, size_t len, long_long_array* arrays)
{
	size_t start = 0, end = 0;
	char ar1 = 0, ar2 = 0, * tmp = input + 7;

	if (len <= 12)
		return funcs_command_error;

	ar1 = input[5];

	if ((sscanf(tmp, "%Iu, %Iu, %c", &start, &end, &ar2)) != 3)
		return funcs_command_error;

	if (!is_letter(ar1) || !is_letter(ar2))
		return funcs_command_error;

	funcs_statuses funcs_s = copy(&(arrays[determine_array_index(ar1)]), start, end, &(arrays[determine_array_index(ar2)]));
	return funcs_s;
}

funcs_statuses do_sort_command(char* input, size_t len, long_long_array* array)
{
	if (len != 7)
		return funcs_command_error;

	if (!(array->count))
		return funcs_empty_arr;

	char ch = input[6];
	if (ch == '+')
		sort(array, compare_non_decreasing);
	else if (ch == '-')
		sort(array, compare_non_increasing);
	else
		return funcs_command_error;
	return funcs_ok;
}

funcs_statuses do_shuffle_command(long_long_array* array)
{
	shuffle(array);
}

funcs_statuses do_stats_command(long_long_array* array)
{
	unsigned respond = stats(stdout, array);
	if (respond == 1)
		return funcs_empty_arr;
	if (respond == -1)
		return funcs_malloc_error;
	return funcs_ok;
}

funcs_statuses do_print_command(char* input, size_t len, long_long_array* array)
{
	size_t start = 0, end = 0;
	char* ptr = input + 8;

	if ((sscanf(ptr, "%Iu, %Iu", &start, &end)) != 2) {
		if (len < 12)
			return funcs_command_error;
		if (to_lower(input[9]) == 'a' && to_lower(input[10] == 'l') && to_lower(input[11]) == 'l') {
			start = 0;
			end = array->count - 1;
		}
		else {
			return funcs_command_error;
		}
	}
	funcs_statuses funcs_s = print_from_to(stdout, *array, start, end);
	return funcs_s;
}
