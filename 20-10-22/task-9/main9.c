#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum sum_statuses {
	sum_malloc_error,
	sum_realloc_error,
	sum_incorrect_notation,
	sum_incorrect_ptr,
	sum_ok
} sum_statuses;

typedef enum do_sum_statuses {
	do_sum_malloc_error,
	do_sum_realloc_error,
	do_sum_ok
} do_sum_statuses;

/// <summary>
/// Function with variable number of arguments, sums arguments after "count" argument, allocates memory for "sum"
/// </summary>
/// <param name="sum">- returned sum value, must be NULL pointer</param>
/// <param name="notation">- notation in which all number must be in</param>
/// <param name="count">- count of passed numbers to sum</param>
/// <param name="">- number to sum of type char*</param>
/// <returns>sum_statuses</returns>
sum_statuses _sum(char** sum, int notation, int count, ...);

/// <summary>
/// Function sums str1 & str2 and returns result in "sum", which is one of str1 or str2
/// </summary>
/// <param name="sum">- a string to return</param>
/// <param name="str1">- first operand, it's length must be >= str2</param>
/// <param name="str2">- second operand, it's lentgh must be <= str1</param>
/// <param name="size1">- length of str1</param>
/// <param name="size2">- length of str2</param>
/// <param name="str1_is_neg">- 1 if str1 is negative number, 0 if positive</param>
/// <param name="str2_is_neg">- 1 if str2 is negative number, 0 if positive</param>
/// <returns>do_sum_statuses</returns>
do_sum_statuses do_sum(char** sum, char* str1, char* str2, int size1, int size2, int str1_is_neg, int str2_is_neg, int notation);

int main()
{
	char s1[] = "F0F", s2[] = "56", s3[] = "-1", s4[] = "-56", s5[] = "-F0A", s6[] = "-f9", * sum = NULL;
	int notation = 10;

	sum_statuses sum_s = _sum(&sum, notation, 2, s2, s2);
	switch (sum_s)
	{
	case sum_malloc_error:
		printf("Memory allocation error happened!\n");
		return -1;
	case sum_realloc_error:
		free(sum);
		printf("Memory reallocation error happened!\n");
		return -2;
	case sum_incorrect_notation:
		printf("Incorrect notation passed!\n");
		return -3;
	case sum_incorrect_ptr:
		printf("Incorrect sum pointer passed!\n");
		return -4;
	default:
		printf("The found sum is: %s\n", sum);
		break;
	}

	return 0;
}

sum_statuses _sum(char** sum, int notation, int count, ...)
{
	if (notation <= 1 || notation > 36) {
		return sum_incorrect_notation;
	}
	if (*sum) {
		return sum_incorrect_ptr;
	}

	int i, sum_size = 0, value_size = 0;
	char* value = NULL, * tmp = NULL;
	va_list ptr;
	va_start(ptr, count);

	for (i = 0; i < count; i++) {
		value = va_arg(ptr, char*);
		value_size = strlen(value);

		if (!(*sum)) { // read the first element
			if (!((*sum) = (char*)malloc(sizeof(char) * (value_size + 1)))) {
				va_end(ptr);
				return sum_malloc_error;
			}
			strcpy((*sum), value);
			sum_size = value_size;
			if (count < 2) {
				va_end(ptr);
				return sum_ok;
			}
			value = va_arg(ptr, char*);
			value_size = strlen(value);
			i++;
		}

		do_sum_statuses do_sum_s;
		if ((sum_size - ((*sum)[0] == '-')) >= (value_size - (value[0] == '-'))) {
			do_sum_s = do_sum(sum, (*sum), value, sum_size, value_size, ((*sum)[0] == '-'), (value[0] == '-'), notation);
		}
		else {
			do_sum_s = do_sum(sum, value, (*sum), value_size, sum_size, (value[0] == '-'), ((*sum)[0] == '-'), notation);
		}
		if (do_sum_s == do_sum_malloc_error || do_sum_s == do_sum_realloc_error) {
			va_end(ptr);
			return sum_realloc_error;
		}
		sum_size = strlen(*sum);
	}

	va_end(ptr);
	return sum_ok;
}


int to_decimal(char ch)
{
	ch = tolower(ch);
	return isdigit(ch) ? ch - '0' : ch - 'a' + 10;
}

char to_ascii(int value)
{
	return (value < 10 ? value + '0' : value + 'a' - 10);
}

void reverse(char* str, int size, int start_ind) {
	int j;
	char tmp;
	for (start_ind, j = size - 1; start_ind < size / 2; start_ind++, j--) {
		tmp = str[start_ind];
		str[start_ind] = str[j];
		str[j] = tmp;
	}
}

int check_nulls(int ind, char* str) {
	int i;
	for (i = ind - 1; i >= 0; i--) {
		if (str[i] != '0') {
			break;
		}
	}
	return i + 1;
}

do_sum_statuses do_sum(char** sum, char* str1, char* str2, int size1, int size2, int str1_is_neg, int str2_is_neg, int notation)
{
	char* res = NULL, * tmp = NULL;
	if (!(res = (char*)malloc(sizeof(char) * (size1 + 3)))) { // for extra number digit + for '-' if exists + for '\0'
		return do_sum_malloc_error;
	}

	int i = size2 - 1, j = size1 - 1, res_ind = 0, partial_sum = 0, add_to_next = 0, str1_digit = 0, str2_digit = 0;

	if (str1_is_neg == str2_is_neg) // both are negative => -(str1 + str2) / positive => (str1 + str2)
	{
		for (j, i; j >= str1_is_neg; j--, i--) {
			partial_sum = to_decimal(str1[j]) + (i >= str2_is_neg ? to_decimal(str2[i]) : 0) + add_to_next;
			res[res_ind++] = to_ascii(partial_sum % notation);
			add_to_next = partial_sum / notation;
		}

		if (add_to_next) {
			res[res_ind++] = '1';
		}

		if (str1_is_neg) {
			res[res_ind++] = '-';
		}

		if (!(tmp = (char*)realloc(res, sizeof(char) * (res_ind + 1)))) {
			free(res);
			return do_sum_realloc_error;
		}
		res[res_ind] = '\0';
	}

	else // one is negative
	{
		for (i, j; j >= str1_is_neg; i--, j--) {
			if (i >= str2_is_neg) {
				str1_digit = to_decimal(str1[j]);
				str2_digit = to_decimal(str2[i]);

				if (str1_digit - str2_digit < 0) { // 10 - 9
					str1[j - 1] = to_ascii(to_decimal(str1[j - 1]) - 1);
					res[res_ind++] = to_ascii(str1_digit + notation - str2_digit);
				}
				else {
					res[res_ind++] = to_ascii(str1_digit - str2_digit);
				}
			}
			else {
				res[res_ind++] = str1[j];
			}
		}

		res_ind = check_nulls(res_ind, res);

		if (!res_ind) // 0
		{
			if (!(tmp = (char*)realloc(res, sizeof(char) * 2))) {
				free(res);
				return do_sum_realloc_error;
			}
			res = tmp;
			res[1] = '\0';
		}
		else {
			if (!(tmp = (char*)realloc(res, sizeof(char) * (res_ind + str1_is_neg + 1)))) {
				free(res);
				return do_sum_realloc_error;
			}
			if (str1_is_neg) {
				res[res_ind++] = '-';
			}
			res[res_ind] = '\0';
		}
	}
	reverse(res, strlen(res), 0);

	free(*sum);
	(*sum) = res;

	return do_sum_ok;
}