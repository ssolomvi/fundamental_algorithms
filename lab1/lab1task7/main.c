#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "header.h"


int main(void)
{
	int arr_size = 0, ind_min = 0, ind_max = 0;
	double* array, max_v = 0, min_v = 0;

	scanf("%d", &arr_size);

	gen_arr_statuses gen_arr_s = gen_arr(&array, arr_size);

	if (gen_arr_s == gen_arr_malloc_error) {
		printf("Allocation error happened\n");
		return 0;
	}

	pr_array(array, arr_size);

	min_max(array, arr_size, &min_v, &max_v, &ind_min, &ind_max);

	array[ind_min] = max_v;
	array[ind_max] = min_v;

	pr_array(array, arr_size);

	free(array);
	return 0;
}
