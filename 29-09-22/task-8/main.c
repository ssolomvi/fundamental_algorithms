#define _CRT_SECURE_NO_WARNINGS
#define VALID_USER_INPUT(x) ((x) == 'a' || (x) == 'b' || (x) == 'c' || (x) == 'd' || (x) == 'e' || (x) == 'x' || (x) == 'p' || (x) == 'h')
#include "header8.h"

void help();

int main(int argc, char** argv)
{
// if a-e return realloc error free array_r
	if (argc != 2) {
		printf("Pls enter path to the file with an array of <= 128 elements as command line argument\n");
		return 0;
	}

	FILE* fi_input;
	if (!(fi_input = fopen(argv[1], "r"))) {
		printf("Open file error\n");
	}

	char user_input[BUFSIZ];
	int exited = 0, size_r = 0, size_new = 0, ind_current;
	double* array_r = NULL, *new_array = NULL;

	odd_statuses odd_s;
	even_statuses even_s;
	farest_statuses farest_s;
	summa_precede_statuses summa_precede_s;
	summa_less_statuses summa_less_s;

	read_arr_statuses read_arr_s = read_arr(fi_input, &array_r, &size_r);
	
	switch (read_arr_s)
	{
	case read_arr_malloc_error:
		printf("Memory allocation error happened\n");
		fclose(fi_input);
		return 0;
	case read_arr_realloc_error:
		printf("Memory reallocation error happened\n");
		fclose(fi_input);
		return 0;
	case read_arr_incorrect_number:
		printf("Incorrect number in file array detected\n");
		fclose(fi_input);
		return 0;
	default:
		printf("An array was successfully read from file\nInput an option:\n");
		break;
	}

	while (!exited)
	{
		scanf("%s", user_input);
		if (strlen(user_input) != 1 || !VALID_USER_INPUT(*user_input))
		{
			printf("Invalid input! Try again... or type h for help\n\n");
			continue;
		}

		switch (*user_input - 97) {\

		case 0:
			odd_s = add_odd_pos(array_r, size_r, &new_array, &size_new);
			switch (odd_s)
			{
			case odd_realloc_error:
				printf("Memory reallocation error happened\n");
				fclose(fi_input);
				free(array_r);
				free(new_array);
				return 0;
			default:
				printf("Elements in odd positions were successfully added to the new array\n");
				break;
			}
			break;

		case 1:
			even_s = add_even(array_r, size_r, &new_array, &size_new);
			switch (even_s)
			{
			case even_realloc_error:
				printf("Memory reallocation error happened\n");
				fclose(fi_input);
				free(array_r);
				free(new_array);
				return 0;
			default:
				printf("Even elements were successfully added to the new array\n");
				break;
			}
			break;

		case 2:
			printf("Enter index of current element:\n");
			scanf("%d", &ind_current);
			farest_s = add_farest_abs(array_r, size_r, &new_array, &size_new, ind_current);
			switch (farest_s)
			{
			case farest_realloc_error:
				printf("Memory reallocation error happened\n");
				fclose(fi_input);
				free(array_r);
				free(new_array);
				return 0;
			default:
				printf("The farest in terms of value element from current was successfully added to the new array\n");
				break;
			}
			break;

		case 3:
			printf("Enter index of current element:\n");
			scanf("%d", &ind_current);
			summa_precede_s = add_summa_precede(array_r, size_r, &new_array, &size_new, ind_current);
			switch (summa_precede_s)
			{
			case summa_precede_realloc_error:
				printf("Memory reallocation error happened\n");
				fclose(fi_input);
				free(array_r);
				free(new_array);
				return 0;
			case summa_precede_type_overflow:
				printf("Error adding summa of precede elements to current one: it grows too big\n");
				break;
			default:
				printf("Summa of precede elements to current one was successfully added to the new array\n");
				break;
			}
			break;

		case 4:
			printf("Enter index of current element:\n");
			scanf("%d", &ind_current);
			summa_less_s = add_summa_less(array_r, size_r, &new_array, &size_new, ind_current);
			switch (summa_less_s)
			{
			case summa_less_realloc_error:
				printf("Memory reallocation error happened\n");
				fclose(fi_input);
				free(array_r);
				free(new_array);
				return 0;
			case summa_less_type_overflow:
				printf("Error adding summa of elements less than current one: it grows too big\n");
				break;
			default:
				printf("Summa of elements less than current one was successfully added to the new array\n");
				break;
			}
			break;

		case 7:
			help();
			break;

		case 15:
			print_array(new_array, size_new);
			break;

		case 23:
			exited++;
			putchar('\n');
			break;
		
		default:
			break;
		}
	}

	free(array_r);
	free(new_array);
	fclose(fi_input);
	return 0;
}

void help()
{
	printf("The programm reads an array <= 128 elements from file which file path is read from command line arguments\n");
	printf("Depending on your choose, program adds to the new array:\n");
	printf("\ta\tnumbers in odd positions from file array\n");
	printf("\tb\teven numbers from file array\n");
	printf("\tc\tthe most distant (by value) value from current number from file array\n");
	printf("\td\tsumma of elements preceding the current number\n");
	printf("\te\tsumma of elements which are less than current value\n");
	printf("\tp\t(option) print a new array\n");
	printf("\tx\t(option) close programm\n");
	printf("In case you choose c-e programm will ask you to eneter index of current element\n");
}
