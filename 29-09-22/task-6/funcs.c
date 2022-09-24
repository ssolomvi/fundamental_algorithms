#include "header6.h"

read_number_statuses read_number(char** number_x, FILE* file_r,  int* is_double)
{
	int max_size = 32, size = 0;
	char c, prev = 0, *tmp;

	(*is_double) = 0;

	if (!((*number_x) = (char*)malloc(max_size * sizeof(char)))) {
		return read_number_alloc_error;
	}

	while (!feof(file_r)) {
		c = fgetc(file_r);

		// number starts
		// ERROR: error reading characters of string
		if (isalnum(c) && (prev == 0 || isspace(prev))) {
			(*number_x)[size++] = c;
		}

		// number continues
		else if ((isalnum(c) || c == '.' || c == ',') && isalnum(prev)) {
			if (size == max_size - 1) {
				max_size *= 2;
				if (!(tmp = (char*)realloc((*number_x), max_size * sizeof(char)))) {
					free(*number_x);
					return read_number_realloc_error;
				}
				(*number_x) = tmp;
			}

			if (c == '.' || c == ',') {
				(*is_double)++;
			}

			(*number_x)[size++] = c;
		}

		// number ends
		else if (isspace(c) && isalnum(prev)) {
			(*number_x)[size] = '\0';
			break;
		}

		prev = c;
	}

	if ((*is_double > 1)) {
		return read_number_incorrect_number;
	}

	return read_number_ok;
}

void to_lower(char** str) {
	int str_len = strlen(*str), i;
	
	for (i = 0; i < str_len; i++) {
		if ((*str)[i] >= 'A' && (*str)[i] <= 'Z') {
			(*str)[i] += ' ';
		}
	}
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

void fnotation(char* number_x, int* not, long double* number_10)
{
	to_lower(&number_x);
	*not = get_notation(number_x);

	if (*not != 0) {
		int i = 0, j, div = 0;
		*number_10 = 0;
		while (number_x[i] != ',' && number_x[i] != '.') {
			div++;
			i++;
		}

		for (i = div - 1, j = 0; i >= 0; i--, j++) {
			*number_10 *= 10;
			if (number_x[j] > '9') {
				*number_x += (number_x[j] - 'a' + 10) * pow((*not), i);
			}
			else {
				*number_x += (number_x[j] - '0') * pow((*not), i);
			}
		}

		for (i = div++, j = 1; i < strlen(number_x); i++, j++) {
			if (number_x[j] > '9') {
				*number_10 += (number_x[i] - 'a' + 10) * pow((*not), -j);
			}
			else {
				*number_10 += (number_x[i] - '0') * pow((*not), -j);
			}
		}
	}
}

void notation(char* number_x, int* not, long* number_10) {
	to_lower(&number_x);
	*not = get_notation(number_x);

	if (*not != 0) {
		int i, j;
		*number_10 = 0;
		for (i = strlen(number_x) - 1, j = 0; i >= 0; i--, j++) {
			if (number_x[j] > '9') {
				*number_10 += (number_x[j] - 'a' + 10) * pow((*not), i);
			}
			else {
				*number_10 += (number_x[j] - '0') * pow((*not), i);
			}
		}
	}
}

void reverse(char** s)
{
	int i, j;
	char c;

	for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
		c = (*s)[i];
		(*s)[i] = (*s)[j];
		(*s)[j] = c;
	}
}

const char* my_itoa(long number)
{
	int size = 0, sign = 0, max_size = 32;
	char* s = (char*)malloc(sizeof(char) * max_size), * tmp;

	if (!s)
		return NULL;

	if (number < 0) {
		sign--; number = -number;
	}
	do {
		if (size == max_size - 1) {
			max_size *= 2;
			if (!(tmp = (char*)realloc(s, max_size * sizeof(char)))) {
				free(s);
				return NULL;
			}
			else {
				s = tmp;
			}
		}

		else {
			s[size++] = number % 10 + '0';
		}
	} while ((number /= 10) > 0);

	if (sign < 0) {
		s[size++] = '-';
	}

	s[size] = '\0';

	if (!(tmp = (char*)realloc(s, (size + 1) * sizeof(char)))) {
		free(s);
		return NULL;
	}

	reverse(&s);
	return s;
}

const char* fmy_itoa(long double number)
{
	int max_size = 1024, eps = 1e-8;
	char* s, * whole_part, * fractional_part;

	if (!(whole_part = my_itoa((long)number))) {
		return NULL;
	}

	int whole_len = strlen(whole_part);

	number -= (long)number;

	while (number - (long)number > eps) {
		number *= 10;
	}

	if (!(fractional_part = my_itoa((long)number))) {
		return NULL;
	}

	s = (char*)malloc(sizeof(char) * (whole_len + strlen(fractional_part) + 2));

	if (!s) {
		return NULL;
	}

	strcpy(s, whole_part);
	strcpy(s + whole_len, ",");
	strcpy(s + whole_len + 1, fractional_part);

	return s;
}
