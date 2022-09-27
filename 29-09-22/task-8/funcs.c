#define _CRT_SECURE_NO_WARNINGS
#include "header8.h"

double my_atoi(char* number) { // returns 89.1234 -> 89.12340...04 ?
	double number_d = 0;
	int i = 0, j, num_len = strlen(number), fract_len = 0, sign = 0;

	if (number[0] == '-') {
		sign++;
		i++;
	}

	for (i; i < num_len; i++) {
		if (number[i] == ',' || number[i] == '.') {
			fract_len = num_len - i - 1;
			continue;
		}
		number_d *= 10;
		number_d += number[i] - '0';
	}

	number_d /= pow(10, fract_len);

	return sign ? -number_d : number_d;
}

read_arr_statuses read_arr(FILE* fi, double** array, int* size)
{
	if (!((*array) = (double*)malloc(sizeof(double) * 128))) {
		return read_arr_malloc_error;
	}

	int number_size = 16, actual_number_size = 0, is_double = 0;
	char ch, _ch = 0, *number = NULL, *tmp;
	double number_d, *tmp_d;
	(*size) = 0;

	while (!feof(fi)) {
		ch = fgetc(fi);

		if ((isdigit(ch) || ch == '-') && (isspace(_ch) || _ch == 0)) {
			// number starts
			if (!(number = (char*)malloc(sizeof(char) * number_size))) {
				free((*array));
				return read_arr_malloc_error;
			}

			number[actual_number_size++] = ch;
		}

		else if ((isdigit(ch) || ch == ',' || ch == '.') && (isdigit(_ch) || _ch == ',' || _ch == '.') || _ch == '-') {
			// number continues
			if (ch == ',' || ch == '.')
				is_double++;

			if (actual_number_size == number_size - 1) {
				number_size *= 2;
				if (!(tmp = (char*)realloc(number, number_size * sizeof(char)))) {
					free(number);
					return read_arr_realloc_error;
				}
				number = tmp;
			}

			number[actual_number_size++] = ch;
		}

		else if (!isdigit(ch) && isdigit(_ch)) {
			//number ends
			if (!(tmp = (char*)realloc(number, sizeof(char) * (actual_number_size + 1)))) {
				free((*array));
				free(number);
				return read_arr_realloc_error;
			}
			
			number = tmp;
			number[actual_number_size] = '\0';

			is_double = 0;
			actual_number_size = 0;

			if (is_double <= 1) {
				number_d = my_atoi(number);
				(*array)[(*size)++] = number_d;

			}
			else {
				free(number);
				return read_arr_incorrect_number;
			}

			free(number);
		}
		_ch = ch;
	}

	if (!(tmp_d = (double*)realloc((*array), sizeof(double) * (*size)))) {
		free(*array);
		return read_arr_realloc_error;
	}

	return read_arr_ok;
}

odd_statuses add_odd_pos(double* array_r, int size_r, double** array_add, int* size_add)	
{
	double* tmp = NULL;
	int num_of_odd_pos = size_r / 2 + size_r % 2;
	if (!((tmp) = (double*)realloc((*array_add), sizeof(double) * ((*size_add) + num_of_odd_pos)))) {
		free((*array_add));
		return odd_realloc_error;
	}

	(*array_add) = tmp;

	int i;

	for (i = 1; i < size_r; i += 2) {
		(*array_add)[(*size_add)++] = array_r[i];
	}

	return odd_ok;
}

void reverse(char** s)
{
	int i, j;
	char c;

	for (i = 0, j = strlen(*s) - 1; i < j; i++, j--) {
		c = (*s)[i];
		(*s)[i] = (*s)[j];
		(*s)[j] = c;
	}
}

char* my_itoa(long number)
{
	int size = 0, sign = 0, max_size = 32;
	char* s = (char*)malloc(sizeof(char) * max_size), * tmp = NULL;

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

char* fmy_itoa(double number)
{
	int _eps = 1e-8, flag_is_null = 0, sign = 0;
	char* s, * whole_part, * fractional_part;
	long number_ld = 0;

	if (number < 0) {
		sign++;
		number = -number;
	}

	if (number - (long)number < eps) {
		return my_itoa(number);
	}

	if (!(whole_part = my_itoa((long)number))) {
		return NULL;
	}

	int whole_len = strlen(whole_part), count_del = 0;

	number -= (long)number;

	while (number - (long)number > _eps && count_del < 7) {
		number *= 10;
		number_ld = number;
		if (!(number_ld % 10)) {
			flag_is_null++;
		}
		if (number_ld % 10) {
			flag_is_null = 0;
		}
		count_del++;

	}

	number_ld /= pow(10, flag_is_null);

	if (!(fractional_part = my_itoa(number_ld))) {
		return NULL;
	}

	if (sign) {
		s = (char*)malloc(sizeof(char) * (whole_len + strlen(fractional_part) + 3));

		if (!s) {
			return NULL;
		}
		s[0] = '-';
	}
	else {
		s = (char*)malloc(sizeof(char) * (whole_len + strlen(fractional_part) + 2));

		if (!s) {
			return NULL;
		}
	}
	strcpy(s + sign, whole_part);
	strcpy(s + sign + whole_len, ",");
	strcpy(s + sign + whole_len + 1, fractional_part);

	return s;
}

int is_even(double number)
{
	char* str_number = fmy_itoa(number);
	return (str_number[strlen(str_number) - 1] - '0') % 2 == 0 ? 1 : 0;
}

even_statuses add_even(double* array_r, int size_r, double** array_add, int* size_add)
{
	double* tmp = NULL;
	if (!(tmp = (double*)realloc((*array_add), sizeof(double) * ((*size_add) + size_r)))) {
		free((*array_add));
		return even_realloc_error;
	}

	(*array_add) = tmp;

	int i;
	for (i = 0; i < size_r; i++) {
		if (is_even(array_r[i])) {
			(*array_add)[(*size_add)++] = array_r[i];
		}
	}

	if (!(tmp = (double*)realloc((*array_add), sizeof(double) * (*size_add)))) {
		free((*array_add));
		return even_realloc_error;
	}
	(*array_add) = tmp;

	return even_ok;
}

farest_statuses add_farest_abs(double* array_r, int size_r, double** array_add, int* size_add, int ind_current)
{
	double* tmp = NULL;
	if (!(tmp = (double*)realloc((*array_add), sizeof(double) * ((*size_add) + 1)))) {
		free(*array_add);
		return farest_realloc_error;
	}

	(*array_add) = tmp;

	int i, ind_abs = -1, farest_value = 0;
	for (i = 0; i < size_r; i++) {
		double dif = abs(array_r[ind_current] - array_r[i]);
		if (dif > farest_value) {
			farest_value = dif;
			ind_abs = i;
		}
	}

	if (ind_abs == -1) {
		ind_abs = ind_current;
	}

	(*array_add)[(*size_add)++] = array_r[ind_abs];

	return farest_ok;
}

summa_precede_statuses add_summa_precede(double* array_r, int size_r, double** array_add, int* size_add, int ind_current)
{
	double* tmp = NULL;
	if (!(tmp = (double*)realloc((*array_add), sizeof(double) * ((*size_add) + 1)))) {
		free(*array_add);
		return summa_precede_realloc_error;
	}

	(*array_add) = tmp;

	double summa = 0;
	int i;

	for (i = 0; i < ind_current; i++) {
		if (DBL_MAX - summa > array_r[i]) {
			summa += array_r[i];
		}
		else {
			free(*array_add);
			return summa_precede_type_overflow;
		}
	}

	(*array_add)[(*size_add)++] = summa;

	return summa_precede_ok;
}

summa_less_statuses add_summa_less(double* array_r, int size_r, double** array_add, int* size_add, int ind_current)
{
	double* tmp = NULL;
	if (!(tmp = (double*)realloc((*array_add), sizeof(double) * ((*size_add) + 1)))) {
		free(*array_add);
		return summa_precede_realloc_error;
	}

	(*array_add) = tmp;

	double summa = 0, curr_element = array_r[ind_current];
	int i;

	for (i = 0; i < size_r; i++) {
		if (curr_element > array_r[i]) {
			if (DBL_MAX - summa > array_r[i]) {
				summa += array_r[i];
			}
			else {
				free(*array_add);
				return summa_less_type_overflow;
			}
		}
	}

	(*array_add)[(*size_add)++] = summa;

	return summa_less_ok;
}

void print_array(double* array, int size)
{
	if (size == 0) {
		printf("The array is empty\n");
	}
	else {


		int i;
		for (i = 0; i < size; i++) {
			printf("%lf ", array[i]);
		}
		putchar('\n');
	}
}
