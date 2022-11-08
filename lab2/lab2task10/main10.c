#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <ctype.h>

typedef enum final_representation_statuses {
	final_representation_incorrect_notation,
	final_representation_incorrect_ptr,
	final_representation_malloc_error,
	final_representation_ok
} final_representation_statuses;

final_representation_statuses is_final_representation(char*** result, int notation, int count, ...);
void print_matrix(char** matrix, int size);
void clear(char*** matrix, int size);

int main()
{
	int notation = 0, count = 3;
	char* num1 = ".12345", * num2 = ".2569", * num3 = ".10001", * num4 = ".8";
	char** res = NULL;

	printf("Enter notation to check if numbers could be dispayed in it as final fractions:\n");
	scanf("%d", &notation);

	final_representation_statuses final_representation_s = is_final_representation(&res, notation, count, num2, num3, num4);
	switch (final_representation_s)
	{
	case final_representation_incorrect_notation:
		printf("Incorrect notation passed in function\n");
		return -1;
	case final_representation_incorrect_ptr:
		printf("Incorrect pointer to an array of strings passed in function\n");
		return -2;
	case final_representation_malloc_error:
		printf("Oops, memory allocation error happened\n");
		return -3;
	default:
		printf("\"0\" means that a number could not be displayed in entered notation as final fraction\n");
		print_matrix(res, count);
		break;
	}

	clear(&res, count);
	return 0;
}

int to_decimal(char ch)
{
	ch = tolower(ch);
	return isdigit(ch) ? ch - '0' : ch - 'a' + 10;
}

char to_ascii(int value)
{
	return (value < 10 ? value + '0' : value + 'A' - 10);
}


void in_commom_fraction(int* numenator, int* denumenator, char* number)
{
	int count = 0, num_len = strlen(number), i = 1;
	(*numenator) = 0; (*denumenator) = 0;
	
	for (i; i < num_len; i++) {
		(*numenator) *= 10;
		(*numenator) += to_decimal(number[i]);
	}

	int mult_2_count = num_len - 1, mult_5_count = num_len - 1;

	while ((!((*numenator) % 2) && mult_2_count > 0) || (!((*numenator) % 5)) && mult_5_count > 0) {
		if (!((*numenator) % 2)) {
			(*numenator) /= 2;
			mult_2_count--;
		}

		if (!((*numenator) % 5)) {
			(*numenator) /= 5;
			mult_5_count--;
		}
	}

	(*denumenator) = pow(2, mult_2_count) * pow(5, mult_5_count);
}

void clear(char*** matrix, int size) {
	for (size -= 1; size >= 0; size--) {
		free((*matrix)[size]);
	}
	free(*matrix);
}

char* f_to_notation(char* number, int notation)
{
	int num_len = strlen(number), size = 16;

	char* res = (char*)malloc(sizeof(char) * (size)), * tmp = NULL;
	if (!res) {
		return NULL;
	}

	int actual_size = 0, i = 1, iterations = 0;

	res[actual_size++] = '.';

	long num_long = 0;
	for (i; i < num_len; i++) {
		num_long *= 10;
		num_long += to_decimal(number[i]);
	}

	long partial = 0;
	while (num_long && iterations < 36) {
		iterations++;
		num_long *= notation;
		partial = pow(10, num_len - 1);

		if (actual_size >= size - 1) {
			size *= 2;
			if (!(tmp = (char*)realloc(res, sizeof(char) * size))) {
				free(res);
				return NULL;
			}
			res = tmp;
		}

		res[actual_size++] = to_ascii(num_long / partial);
		num_long %= partial;
	}

	if (!(tmp = (char*)realloc(res, sizeof(char) * (actual_size + 1)))) {
		free(res);
		return NULL;
	}

	res = tmp;
	res[actual_size] = '\0';
	return res;
}

final_representation_statuses is_final_representation(char*** result, int notation, int count, ...)
{
	if (notation <= 1 || notation > 36) {
		return final_representation_incorrect_notation;
	}

	if ((*result)) {
		return final_representation_incorrect_ptr;
	}
	else {
		if (!((*result) = (char**)calloc(count, sizeof(char*)))) {
			return final_representation_malloc_error;
		}
	}

	va_list ptr;
	va_start(ptr, count);

	int i = 0, numenator = 0, denumenator = 0, is_endless = 0, result_ind = 0;
	char* number = 0;

	// 1) .8 -> 4/5
	// 2) find denumenator % [simple_numbers] == 0
	// 3) check if notation % [simple_numbers] == 0

	for (i = 0; i < count; i++) {
		number = va_arg(ptr, char*);
		is_endless = 0;
		in_commom_fraction(&numenator, &denumenator, number);
		if (!(denumenator % 2) && (notation % 2 != 0))
			is_endless++;
		if (!(denumenator % 5) && (notation % 5 != 0))
			is_endless++;

		if (is_endless) {
			if (!((*result)[result_ind] = (char*)calloc(2, sizeof(char)))) {
				clear(result, result_ind);
				va_end(ptr);
				return final_representation_malloc_error;
			}
			strcpy((*result)[result_ind++], "0");
		}
		else {
			if (!((*result)[result_ind++] = f_to_notation(number, notation))) {
				clear(result, result_ind);
				va_end(ptr);
				return final_representation_malloc_error;
			}
		}
	}
	
	va_end(ptr);
	return final_representation_ok;
}

void print_matrix(char** matrix, int size)
{
	int i = 0;
	for (i; i < size; i++) {
		printf("%s ", matrix[i]);
	}
	printf("\n\n");
}