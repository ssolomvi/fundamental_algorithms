#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

void find_l_ones(int k, int count_1, int** array, int* array_num);

void find_l_in_a_row(int k, int l, int** array, int* array_num);

void print_array_stream(int* array, int array_num);

int main()
{
	int k = 0, l = 0; // k -- size in bytes, l -- count of '1'
	printf("Enter k: count of digits and l: count of '1' in a row:\n>>> ");
	if ((scanf("%d %d", &k, &l) != 2)) {
		printf("Incorrect data entered!\n");
		return -1;
	}

	int* array = NULL, array_num = 0;
	find_l_ones(k, l, &array, &array_num);
	if (!array) {
		printf("Some of below errors happened in function find_l_ones:\n");
		printf("\tpassed l is > k\n\tpassed array ptr is not NULL\n\tmemory allocation\\reallocation error happened\n");
		return -2;
	}

	printf("Where do you want to get the results of function calculating numbers with exactly %d '1'?\n>>> ", l);
	print_array_stream(array, array_num);
	free(array);
	array = NULL;

	find_l_in_a_row(k, l, &array, &array_num);
	if (!array && l != 0) {
		printf("Some of below errors happened in function find_l_in_a_row:\n");
		printf("\tpassed l is > k\n\tpassed array ptr is not NULL\n\tmemory allocation\\reallocation error happened\n");
		return -3;
	}

	printf("Where do you want to get the results of function calculating numbers with exactly %d '1' in a row?\n>>> ", l);
	print_array_stream(array, array_num);
	free(array);

	return 0;
}

unsigned long long factorial(int number)
{
	unsigned long long res = 1;
	while (number) {
		res *= number;
		number--;
	}
	return res;
}

unsigned long long shrink_factorial(int max, int min)
{
	unsigned long long res = 1;
	int i;
	for (i = max; i > min; i--) {
		res *= i;
	}
	return res;
}


void perm_with_repeat(int count_1, int count_0, int* array, int* array_num, int number, int next)
{
	if ((!count_1 && next) || (!count_0 && !next)) {
		return;
	}

	number <<= 1;
	
	if (next) {
		number |= 1;
		count_1--;
	}
	else {
		count_0--;
	}

	if (count_0 == 0 && count_1 == 0) {
		array[(*array_num)++] = number;
		return;
	}

	perm_with_repeat(count_1, count_0, array, array_num, number, 0);
	perm_with_repeat(count_1, count_0, array, array_num, number, 1);
}

void find_l_ones(int k, int count_1, int** array, int* array_num)
{
	(*array_num) = 0;

	if (count_1 > k || count_1 < 0 || k < 0) {
		(*array) = NULL;
		return;
	}

	if (count_1 == 0 && k != 0) {
		if (!((*array) = (int*)malloc(sizeof(int)))) {
			(*array) = NULL;
			return;
		}
		(*array_num)++;
		(**array) = 0;
		return;
	}

	if ((*array) != NULL) { // *array must be NULLptr
		(*array) = NULL;
		return;
	}

	int count_0 = k - count_1, number = 0, max = (count_1 > count_0 ? count_1 : count_0);
	unsigned long long count_of_perm_with_repeat = shrink_factorial(k, max) / factorial(k - max);

	if (!((*array) = (int*)malloc(sizeof(int) * count_of_perm_with_repeat))) {
		(*array) = NULL;
		return;
	}

	perm_with_repeat(count_1, count_0, *array, array_num, number, 0);
	perm_with_repeat(count_1, count_0, *array, array_num, number, 1);
}

void find_l_in_a_row(int k, int l, int** array, int* array_num)
{
	if (l > k || l < 0 || k < 0) {
		(*array) = NULL;
		return;
	}

	if ((*array) != NULL) { // *array must be NULLptr
		(*array) = NULL;
		return;
	}

	if (!((*array) = (int*)malloc(sizeof(int)))) {
		(*array) = NULL;
		return;
	}

	if (!k) {
		return;
	}

	(**array) = 0;
	(*array_num) = 1;

	if (l == 0 && k != 0) {
		return;
	}

	for (int i = k; i > 0; i--) {
		(**array) <<= 1;
		if (l) {
			(**array) |= 1;
			l--;
		}
	}
}

void print_array(FILE* stream, int* array, int arr_size)
{
	if (!arr_size) {
		fprintf(stream, "An array of numbers is empty\n");
	}
	int i;
	for (i = 0; i < arr_size; i++) {
		fprintf(stream, "%d ", array[i]);
	}
	fprintf(stream, "\n\n");
}

void print_array_stream(int* array, int array_num)
{
	char* stream[BUFSIZ];
	
	if (scanf("%s", stream) != 1 || !strcmp(stream, "")) {
		printf("Incorrect stream has been chosen!\n");
		return;
	}

	if (!strcmp(stream, "stdout")) {
		print_array(stdout, array, array_num);
	}
	else {
		FILE* fi;
		if (!(fi = fopen(stream, "w"))) {
			printf("Error opening file!\n");
			return -3;
		}
		print_array(fi, array, array_num);
		fclose(fi);
		printf("The results were saved in %s file\n", stream);
	}
}
