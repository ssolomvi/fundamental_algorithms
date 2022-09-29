#define _CRT_SECURE_NO_WARNINGS
#include "header9.h"

int main(void)
{
	int notation, actual_size_array = 0, i;

	printf("Enter notation:\n");
	scanf("%d", &notation);

	if (notation < 2 || notation > 36) {
		printf("Incorrect notation! It should be in range [2;36]\n");
		return 0;
	}

	float *array = NULL, max = 0;

	printf("Now enter number in this notation, use Stop to stop:\n");
	read_arr_statuses read_arr_s = read_array(&array, &actual_size_array, notation);
	switch (read_arr_s)
	{
	case read_arr_incorrect_notation:
		printf("One of entered numbers is of incorrect notation\n");
		free(array);
		return 0;
	case read_arr_allocation_error:
		printf("Memory allocation error happened\n");
		free(array);
		return 0;
	case read_arr_reallocation_error:
		printf("Memory reallocation error happened\n");
		free(array);
		return 0;
	default:
		printf("\nAll the numbers were read successfully!\n");
		break;
	}

	find_max_abs_statuses find_max_abs_s = find_max_abs(array, actual_size_array, &max);
	if (find_max_abs_s == find_max_abs_array_empty) {
		printf("Oops... There no enetered numbers. Try again!\n");
		return 0;
	}

	char* num_9 = NULL, * num_18 = NULL, * num_27 = NULL, * num_36 = NULL;

	convert_to_statuses convert_s;

	for (i = 1; i < 5; i++) {
		if (!num_9) 
			convert_s = convert(max, 9 * i, &num_9);
		else if (!num_18)
			convert_s = convert(max, 9 * i, &num_18);
		else if (!num_27)
			convert_s = convert(max, 9 * i, &num_27);
		else if (!num_36)
			convert_s = convert(max, 9 * i, &num_36);

		switch (convert_s)
		{
		case conv_malloc_error:
			printf("Memory allocation error happened\n");
			return 0;
		case conv_realloc_error:
			printf("Memory reallocation error happened\n");
			return 0;
		default:
			break;
		}
	}

	printf("Found max number: %lf\nIn 9 notataion: %s\nIn 18 notataion: %s\nIn 27 notataion: %s\nIn 36 notataion: %s\n", max, num_9, num_18, num_27, num_36);
	free(array);
	free(num_9);
	free(num_18);
	free(num_27);
	free(num_36);
	return 0;
}