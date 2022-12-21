#include "header4.h"
#define count_of_commands 11

// TODO: len is length of command WITHOUT '\n'

typedef struct input_properties {
	int command;
	char array1;
	char array2;
	size_t start_index;
	size_t end_index;
	char* filename;
} input_properties;

void help();
void count_commands(unsigned** commands);
int handle_responses(funcs_statuses funcs_s);
int parse(char* input);

int main(void)
{
	FILE* in = NULL, * out = NULL;
	char user_input[BUFSIZ] = { 0 };
	char user_stream[BUFSIZ];
	
	int exited = 0, func_respond;
	funcs_statuses funcs_s = -1;
	long_long_array* arrays = (long_long_array*)malloc(sizeof(long_long_array) * 26);
	unsigned* commands = (unsigned*)calloc(count_of_commands, sizeof(unsigned));

	if (!arrays || !commands) {
		handle_responses(funcs_malloc_error);
		return -1;
	}

	help();

	while (!exited)
	{
		printf(">>> ");
		fgets(user_input, BUFSIZ, stdin);
		printf("Invalid input! Try again...\n\n");

	}

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
	printf("\tCopy <array_name1>, <start index>, <end index>, <array_name2>\n");
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
		return -3;
	case funcs_realloc_error:
		printf("Memory realloaction error happened\n");
		return -4;
	case funcs_open_file_error:
		printf("Error opening file\n");
		return -5;
	default:
		break;
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

unsigned sum_word(char* str, size_t length, unsigned* end_command)
{
	unsigned sum = 0;
	size_t i = 0;
	for (i = 0; i < length; i++) {
		if (is_letter(str[i]))
			sum += to_lower(str[i]);
		else
			break;
	}
	(*end_command) = i;
	return sum;
}

void count_commands(unsigned** commands)
{
	unsigned tmp = 0;
	(*commands)[0] = sum_word("Load", 4, &tmp);
	(*commands)[1] = sum_word("Save", 4, &tmp);
	(*commands)[2] = sum_word("Rand", 4, &tmp);
	(*commands)[3] = sum_word("Concat", 6, &tmp);
	(*commands)[4] = sum_word("Free", 4, &tmp);
	(*commands)[5] = sum_word("Remove", 6, &tmp);
	(*commands)[6] = sum_word("Copy", 4, &tmp);
	(*commands)[7] = sum_word("Sort", 4, &tmp);
	(*commands)[8] = sum_word("Shuffle", 7, &tmp);
	(*commands)[9] = sum_word("Stats", 5, &tmp);
	(*commands)[10] = sum_word("Print", 5, &tmp);
}

funcs_statuses do_load_command(char* input, size_t len, long_long_array* array)
{
	char filename[FILENAME_MAX] = {0}, tmp = 0;
	if (len <= 8)
		return funcs_command_error;

	size_t i, j;
	for (i = 8, j = 0; i < len; j++, i++) {
		filename[j] = input[i];
	}
	filename[j] = '\0';
	
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
	filename[j] = '\0';

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

	if ((sscanf(input, "%Iu, %d, %d", &count, &lb, &rb)) != 3)
		return funcs_command_error;

	funcs_statuses funcs_s = load_rand(array, count, lb, rb);
	return funcs_s;
}

unsigned determine_array_index(char ch)
{
	return (to_lower(ch) - 'a');
}

funcs_statuses do_concat_command(char* input, size_t len, long_long_array* arrays)
{
	char first = 0, second = 0, *ptr = input + 7;
	if ((len <= 9))
		return funcs_command_error;
	if (!is_letter((first = input[7])) || !is_letter((second = input[9])))
		return funcs_command_error;

	funcs_statuses funcs_s = concat(&(arrays[determine_array_index(first)]), &(arrays[determine_array_index(second)]));
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
	if ((sscanf(input, "%Iu, %Iu", &start, &end)) != 2)
		return funcs_command_error;

	funcs_statuses funcs_s = remove_from_arr(array, start, end);
	return funcs_s;
}

funcs_statuses do_copy_command(char* input, size_t len, long_long_array* arrays)
{
	size_t start = 0, end = 0;
	char ar1 = 0, ar2 = 0, *tmp = input + 7;

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
	if (stats(stdout, array) != 0)
		return funcs_malloc_error;
	return funcs_ok;
}

funcs_statuses do_print_command(char* input, size_t len, long_long_array* array)
{
	size_t start = 0, end = 0;

	if ((sscanf(input, "%Iu, %Iu", &start, &end)) != 2) {
		if (len <= 12)
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

int parse(unsigned* commands, char* input, input_properties* output)
{
	(*output).array1 = 0;
	(*output).array2 = 0;
	(*output).command = 0;
	(*output).start_index = 0;
	(*output).end_index = 0;
	(*output).filename = NULL;

	size_t input_len = strlen(input), i = 0;
	unsigned command_end_index = 0, command = sum_word(input, input_len, &command_end_index), 
		is_command = 0;
	
	for (i = 0; i < count_of_commands; i++) {
		if (command == commands[i]) {
			is_command = 1;
			break;
		}
	}

	if (!is_command)
		return -1;

	(*output).command = command;
	(*output).array1 = to_lower(input[++command_end_index]);

	if (!is_letter((*output).array1))
		return -2;

	if (command = commands[4]) // free
		return 0;


}