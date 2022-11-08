#include "header.h"

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

gen_arr_statuses gen_arr(double** array, int size)
{
	int i, rand_case;

	if (!((*array) = (double*)malloc(sizeof(double) * size))) {
		return gen_arr_malloc_error;
	}

	srand(time(NULL));

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

	return gen_arr_ok;
}

void min_max(double* array, int arr_size, double* min, double* max, int* ind_min, int* ind_max)
{
	int i;

	(*min) = DBL_MAX;
	(*max) = DBL_MIN;
	
	for (i = 0; i < arr_size; i++) {
		if (array[i] < (*min)) {
			(*min) = array[i];
			(*ind_min) = i;
		}

		if (array[i] > (*max)) {
			(*max) = array[i];
			(*ind_max) = i;
		}
	}
}

void pr_array(double* array, int arr_size)
{
	int i;
	for (i = 0; i < arr_size; i++) {
		printf("%lf ", array[i]);
	}
	printf("\n\n");
}
