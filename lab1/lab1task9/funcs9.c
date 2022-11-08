#define _CRT_SECURE_NO_WARNINGS
#include "header9.h"

void to_lower(char* str) {
	int str_len = strlen(str), i;

	for (i = 0; i < str_len; i++) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			str[i] += ' ';
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

float to_decimal(char* number_str, int notation)
{
	int i, j, num_str_len = strlen(number_str), fract_len = 0, sign = 0;
	float result = 0;

	if (number_str[0] == '-')
		sign++;

	// find out index of '.' if exists
	for (i = sign; i < num_str_len; i++) {
		if (number_str[i] == ',' || number_str[i] == '.') {
			fract_len = num_str_len - i - 1;
			break;
		}
	}

	for (i = i - sign - 1, j = sign; i >= 0; i--, j++) {
		result += (number_str[j] - (isdigit(number_str[j]) ? '0' : ('a' - 10))) * pow(notation, i);
	}

	for (++j, i = 1; i <= fract_len; j++, i++) {
		result += (number_str[j] - (isdigit(number_str[j]) ? '0' : ('a' - 10))) * pow(notation, -i);
	}

	return sign ? -result : result;
}

read_arr_statuses read_array(float** array, int* actual_size_array, int notation)
{
	int size_array = 16, act_notation;
	char number_str[32];
	float *tmp = NULL;

	if (!((*array) = (float*)malloc(sizeof(float) * size_array))) {
		return read_arr_allocation_error;
	}
	
	(*actual_size_array) = 0;

	while (scanf("%s", number_str)) {
		if (!strcmp(number_str, "Stop")) {
			if (!(tmp = (float*)realloc((*array), sizeof(float) * (*actual_size_array)))) {
				printf("Memory reallocation error\n");
				free((*array));
				return 0;
			}
			(*array) = tmp;
			break;
		}

		if ((*actual_size_array) == size_array - 1) {
			size_array *= 2;
			if (!(tmp = (float*)realloc((*array), sizeof(float) * size_array))) {
				printf("Memory reallocation error\n");
				free((*array));
				return 0;
			}
			(*array) = tmp;
		}

		to_lower(&number_str);

		act_notation = get_notation(number_str);
		if (act_notation > notation) {
			return read_arr_incorrect_notation;
		}

		(*array)[(*actual_size_array)++] = to_decimal(number_str, notation); // 134.723969 when 134.735532 is correct
	}
}

find_max_abs_statuses find_max_abs(float* array, int size, float* max)
{
	if (size == 0) {
		return find_max_abs_array_empty;
	}

	int i;
	(*max) = 0;

	for (i = 0; i < size; i++) {
		if (abs((*max)) - abs(array[i]) < eps) {
			(*max) = array[i];
		}
	}

	return find_max_abs_ok;
}

void reverse(char** string)
{
	int i, j, str_len = strlen((*string));
	char tmp;
	for (i = 0, j = str_len - 1; i < str_len / 2; j--, i++) {
		tmp = (*string)[i];
		(*string)[i] = (*string)[j];
		(*string)[j] = tmp;
	}
}

convert_to_statuses convert(float number_f, int notation, char** number_str)
{
	int size_num = 16, actual_size = 0, number_d = (int)number_f, rem, iter = 0, whole_part;
	char* tmp = NULL, rem_c, whole_part_c;

	(*number_str) = (char*)malloc(sizeof(char) * size_num);
	if (!(*number_str))
		return conv_malloc_error;

	// convert whole part of number
	do {
		rem = number_d % notation;
		if (rem < 10)
		{
			rem_c = rem + '0';
		}
		else {
			rem_c = rem + 'a' - 10;
		}

		if (actual_size == size_num - 1) {
			size_num *= 2;
			if (!(tmp = (char*)realloc((*number_str), sizeof(char) * size_num))) {
				free((*number_str));
				return conv_realloc_error;
			}
			(*number_str) = tmp;
		}
		(*number_str)[actual_size++] = rem_c;

		number_d /= notation;
	} while (number_d > 0);

	(*number_str)[actual_size] = '\0';
	reverse(number_str);

	if (number_f - (int)number_f > eps) {

		(*number_str)[actual_size++] = '.';
		number_f -= (int)number_f;

		while (iter < 7 && number_f - (int)number_f > eps) {
			number_f *= notation;
			whole_part = (int)number_f;
			if (whole_part < 10) {
				whole_part_c = whole_part + '0';
			}
			else {
				whole_part_c = whole_part + 'a' - 10;
			}

			if (actual_size == size_num - 1) {
				size_num *= 2;
				if (!(tmp = (char*)realloc((*number_str), sizeof(char) * size_num))) {
					free((*number_str));
					return conv_realloc_error;
				}
				(*number_str) = tmp;
			}
			(*number_str)[actual_size++] = whole_part_c;

			number_f -= (int)number_f;
			iter++;
		}
	}

	(*number_str)[actual_size++] = '\0';

	if (!(tmp = (char*)realloc((*number_str), sizeof(char) * actual_size))) {
		free((*number_str));
		return conv_realloc_error;
	}
	(*number_str) = tmp;

	return conv_ok;
}