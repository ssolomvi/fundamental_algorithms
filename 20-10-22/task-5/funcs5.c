#include "header5.h"

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

void min_max(double* array, int arr_size, int* ind_min, int* ind_max)
{
	int i;

	double min = DBL_MAX;
	double max = DBL_MIN;

	for (i = 0; i < arr_size; i++) {
		if (array[i] < min) {
			min = array[i];
			(*ind_min) = i;
		}

		if (array[i] > max) {
			max = array[i];
			(*ind_max) = i;
		}
	}
}

void swap(double* num1, double* num2)
{
	double tmp;
	tmp = (*num1);
	(*num1) = (*num2);
	(*num2) = tmp;
}

make_uniq_statuses make_uniq(double* init_array, int size, double** new_array, int* size_uniq)
{
	if (!((*new_array) = (double*)malloc(sizeof(double) * size))) {
		return make_uniq_malloc_error; // TODO: free init_array
	}

	int i, j, has_equals;
	(*size_uniq) = 0;
	for (i = 0; i < size; i++) {
		has_equals = 0;
		for (j = 0; j < size; j++) {
			if (i == j) {
				continue;
			}
			if (abs(init_array[i] - init_array[j]) < eps) {
				has_equals = 1;
				break;
			}
		}

		if (!has_equals) {
			(*new_array)[(*size_uniq)++] = init_array[i];
		}
	}


	double* tmp;
	if (!(tmp = (double*)realloc((*new_array), sizeof(double) * (*size_uniq)))) {
		free(*new_array);
		return make_uniq_realloc_error;
	}

	(*new_array) = tmp;

	return make_uniq_ok;
}

void print_array(double* array, int arr_size)
{
	int i;
	for (i = 0; i < arr_size; i++) {
		printf("%lf ", array[i]);
	}
	printf("\n\n");
}	