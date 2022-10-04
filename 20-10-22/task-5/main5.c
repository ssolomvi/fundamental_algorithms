#define _CRT_SECURE_NO_WARNINGS
#include "header5.h"

int main(void)
{
	srand(time(NULL));

	int min, max;
	printf("Enter min and max of the range of values in array:\t");
	scanf("%d %d", &min, &max);
	
	int len = rand() % max + min, ind_min = 0, ind_max = 0;
	double* array = NULL;

	gen_arr_statuses gen_arr_s = gen_arr(&array, len);
	if (gen_arr_s == gen_arr_malloc_error) {
		printf("Memory allocation error happened\n");
		return 0;
	}
		
	printf("Eneter what to do with an array\n");
	printf("\t-s  swap minimum and maximum of an array\n");
	printf("\t-u  create a new array consisting of unique elements of initial array\nYou choose:\t");
	char* user_decision[10];
	scanf("%s", user_decision);
	
	if (!strcmp(user_decision, "-s")) {
		printf("The initial array:\n");
		print_array(array, len);
		min_max(array, len, &ind_min, &ind_max);
		swap(&(array[ind_min]), &(array[ind_max]));
		printf("The result array:\n");
		print_array(array, len);
	}
	
	if (!strcmp(user_decision, "-u")) {
		printf("The initial array of size %d:\n", len);
		print_array(array, len);
		double* uniq_array = NULL;
		int len_uniq_array = 0;
		make_uniq_statuses make_uniq_s = make_uniq(array, len, &uniq_array, &len_uniq_array);
		if (make_uniq_s == make_uniq_malloc_error || make_uniq_s == make_uniq_realloc_error) {
			free(array);
			printf("Memory allocation error\n");
			return 0;
		}
		else if (make_uniq_s == make_uniq_realloc_error) {
			free(array);
			free(uniq_array);
			printf("Memory reallocation error\n");
			return 0;
		}

		printf("The result array of size %d:\n", len_uniq_array);
		print_array(uniq_array, len_uniq_array);
		free(uniq_array);
	}

	free(array);
	return 0;
}
