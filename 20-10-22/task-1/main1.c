#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>

int gen_arr(double** array, int size);

void print_array(double* array, int size);

int main(void)
{
	srand(time(NULL));
	int len1 = rand() % 100 + 1, len2 = rand() % 100 + 1, response = 0, i, j, max_len;
	double* array1, * array2, * array3;
	
	response += gen_arr(&array1, len1);
	response += gen_arr(&array2, len2);
	if (response) {
		printf("Memory allocation error happened\n");
		return 0;
	}
	printf("The 1st array:\n");
	print_array(array1, len1);
	printf("The 2nd array:\n");
	print_array(array2, len2);


	if (!(array3 = (int*)calloc(len1, sizeof(double)))) {
		printf("Memory allocation error happened\n");
		return 0;
	}

	for (i = 0; i < len1; i++) {
		array3[i] = array1[i] + array2[i];
		if (i == len2 - 1) {
			break;
		}
		if (i == len1 - 1) {
			print_array(array3, len1);
			free(array1);
			free(array2);
			free(array3);
			return 0;
		}
	}

	if (len1 != len2)
	{
		for (j = i + 1; j < len1; j++) {
			if (DBL_MAX - array3[j] < array1[j] + array2[i]) {
				printf("Type overflow; genereted numbers were too big\n");
				free(array1);
				free(array2);
				free(array3);
				return 0;
			}
			array3[j] = array1[j] + array2[i];
		}
	}

	printf("The result array:\n");
	print_array(array3, len1);

	free(array1);
	free(array2);
	free(array3);

	return 0;
}


int gen_whole_non_negative()
{
	int range_length, shift, number;

	range_length = rand();
	shift = rand();

	return (number = rand() % range_length + shift);
}

int gen_whole_negative()
{
	int range_length, shift, tmp, number;

	range_length = rand();
	shift = rand();

	if (range_length > shift) {
		tmp = range_length;
		range_length = shift;
		shift = tmp;
	}

	return (number = rand() % range_length - shift);
}

double gen_double_non_negative()
{
	double number;
	int whole_part = gen_whole_non_negative();

	return (number = whole_part + ((double)rand()) / RAND_MAX);
}

double gen_double_negative()
{
	double number;
	int whole_part = gen_whole_negative();

	return (number = whole_part + ((double)rand()) / RAND_MAX);
}

int gen_arr(double** array, int size)
{
	int i, rand_case;

	if (!((*array) = (double*)malloc(sizeof(double) * size))) {
		return -1;
	}

	for (i = 0; i < size; i++) {
		rand_case = rand() % 4 + 1;

		switch (rand_case)
		{
		case 1:
			// gen whole >= 0 num
			(*array)[i] = gen_whole_non_negative();
			break;

		case 2:
			// gen whole <= 0 num
			(*array)[i] = gen_whole_negative();
			break;

		case 3:
			(*array)[i] = gen_double_non_negative();
			break;

		case 4:
			(*array)[i] = gen_double_negative();
			break;
		default:
			(*array)[i] = 0;
			break;
		}

	}

	return 0;
}

void print_array(double* array, int size)
{
	int i;
	for (i = 0; i < size; i++) {
		printf("%lf ", array[i]);
	}
	printf("\n\n");
}
