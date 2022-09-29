#define _CRT_SECURE_NO_WARNINGS
#include "header6.h"

read_number_statuses read_number(char** number_x, FILE* file_r)
{
	int max_size = 32, size = 0;
	char c, prev = 0, * tmp;

	if (!((*number_x) = (char*)malloc(max_size * sizeof(char)))) {
		return read_number_alloc_error;
	}

	while (!feof(file_r)) {
		c = fgetc(file_r);

		// number starts
		if (isalnum(c) && (prev == 0 || isspace(prev))) {
			if (c >= 'A' && c <= 'Z') {
				c += ' ';
			}
			(*number_x)[size++] = c;
		}

		// number continues
		else if (isalnum(c) && isalnum(prev)) {
			if (size == max_size - 1) {
				max_size *= 2;
				if (!(tmp = (char*)realloc((*number_x), max_size * sizeof(char)))) {
					free(*number_x);
					return read_number_realloc_error;
				}
				(*number_x) = tmp;
			}

			if (c >= 'A' && c <= 'Z') {
				c += ' ';
			}

			(*number_x)[size++] = c;
		}

		// number ends
		else if ((isspace(c) || c == EOF) && isalnum(prev)) {
			(*number_x)[size++] = '\0';
			if (!(tmp = (char*)realloc((*number_x), sizeof(char) * size))) {
				free((*number_x));
				return read_number_realloc_error;
			}
			(*number_x) = tmp;
			break;
		}

		prev = c;
	}

	return read_number_ok;
}

int get_notation(char* number)
{
	int not = 2, i, max_ascii = 0;
	for (i = 0; i < strlen(number); i++) {
		if (max_ascii < (int)number[i]) {
			max_ascii = (int)number[i];
		}
	}

	if (max_ascii >= (int)'0' && max_ascii <= (int)'9') {
		if (max_ascii == (int)'0')
			return 2;
		return max_ascii - 47; // ASCII: 1 -- 49
	}
	else if (max_ascii >= (int)'a' && max_ascii <= (int)'z') {
		return max_ascii - 86; // ASCII: a -- 97
	}
	else {
		return 0;
	}
}

long long to_decimal(char* number_str, int notation)
{
	int i, j, num_str_len = strlen(number_str), sign = 0;
	long long result = 0;

	if (number_str[0] == '-')
		sign++;

	for (i = sign; i < num_str_len; i++) {
	}

	for (i = i - sign - 1, j = sign; i >= 0; i--, j++) {
		result += (number_str[j] - (isdigit(number_str[j]) ? '0' : ('a' - 10))) * pow(notation, i);
	}

	return sign ? -result : result;
}

void reverse(char* s)
{
	int i, j;
	char c;

	for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
		c = (s)[i];
		(s)[i] = (s)[j];
		(s)[j] = c;
	}
}

my_itoa_statuses my_itoa(long long number, char** number_str)
{
	int size = 0, sign = 0, max_size = 32;
	if (!((*number_str) = (char*)malloc(sizeof(char) * max_size))) {
		return my_itoa_malloc_error;
	}
	char* tmp;

	if (number < 0) {
		sign--; number = -number;
	}
	do {
		if (size == max_size - 1) {
			max_size *= 2;
			if (!(tmp = (char*)realloc((*number_str), max_size * sizeof(char)))) {
				free((*number_str));
				return my_itoa_realloc_error;
			}
			(*number_str) = tmp;
		}

		else {
			(*number_str)[size++] = number % 10 + '0';
		}
	} while ((number /= 10) > 0);

	if (sign < 0) {
		(*number_str)[size++] = '-';
	}

	(*number_str)[size++] = '\0';

	if (!(tmp = (char*)realloc((*number_str), size * sizeof(char)))) {
		free((*number_str));
		return my_itoa_realloc_error;
	}
	(*number_str) = tmp;

	reverse(*number_str);
	return my_itoa_ok;
}
