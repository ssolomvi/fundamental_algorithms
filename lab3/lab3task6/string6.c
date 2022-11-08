#include "string6.h"

size_t length_string(char* str)
{
	size_t counter = 0;
	while (*str != '\0') {
		counter++;
		str++;
	}
	return counter;
}

void print_string(FILE* stream, string str)
{

	int i;
	for (i = 0; i < str.length; i++) {
		fputc(str.str[i], stream);
	}
	fputc('\n', stream);
}

void print_string_v2(FILE* stream, string str)
{
	fprintf(stream, "%s\n", str.str);
}

string create_string(FILE* stream, size_t to_allocate, char divider)
{   // if to_allocate == 0, reads up to '\n'
	string new_str;
	char* tmp = NULL;

	if (to_allocate) {
		if (!(new_str.str = (char*)malloc(sizeof(char) * to_allocate))) {
			new_str.str = NULL;
			new_str.length = 0;
			return new_str;
		}

		if (!fgets(new_str.str, to_allocate, stream)) {
			free(new_str.str);
			new_str.str = NULL;
			new_str.length = 0;
			return new_str;
		}

		new_str.length = length_string(new_str.str);

		if (new_str.str[new_str.length - 1] == divider) {
			if (!(tmp = (char*)realloc(new_str.str, sizeof(char) * new_str.length))) {
				free(new_str.str);
				new_str.str = NULL;
				new_str.length = 0;
				return new_str;
			}
			new_str.str = tmp;
			new_str.length--;
			new_str.str[new_str.length] = '\0';
			return new_str;
		}

		if (!(tmp = (char*)realloc(new_str.str, sizeof(char) * (new_str.length + 1)))) {
			free(new_str.str);
			new_str.str = NULL;
			new_str.length = 0;
			return new_str;
		}
		new_str.str = tmp;

		return new_str;
	}

	size_t size = 16, actual_size = 0;
	char ch = 0;

	if (!(new_str.str = (char*)malloc(sizeof(char) * size))) {
		new_str.str = NULL;
		new_str.length = 0;
		return new_str;
	}

	while ((ch = fgetc(stream)) != divider) {
		if (actual_size >= size - 1) {
			size *= 2;
			if (!(tmp = (char*)realloc(new_str.str, sizeof(char) * size))) {
				new_str.str = NULL;
				new_str.length = 0;
				return new_str;
			}
			new_str.str = tmp;
		}

		new_str.str[actual_size++] = ch;
	}

	if (!(tmp = (char*)realloc(new_str.str, sizeof(char) * (actual_size + 1)))) {
		new_str.str = NULL;
		new_str.length = 0;
		return new_str;
	}
	new_str.str = tmp;

	new_str.str[actual_size] = '\0';
	new_str.length = actual_size;
	return new_str;
}

void delete_string(string* str)
{
	if (!str) {
		return;
	}
	str->length = 0;

	if (!(str->str)) {
		return;
	}

	free(str->str);
	str->str = NULL;
}

int compare_strings(string first, string second, int compare(string, string))
{
	if (!first.length && !second.length) {
		return 0;
	}

	return compare(first, second);
}

void copy_string(string* copy, string original)
{
	if (copy->str) {
		delete_string(copy);
	}

	char* copy_new_str = NULL;
	int i;

	if (!original.length || !original.str) {
		copy->length = 0;
		copy->str = NULL;
		return;
	}

	if (!(copy_new_str = (char*)malloc(sizeof(char) * (original.length + 1)))) {
		copy->length = nan;
		copy->str = NULL;
		return;
	}

	for (i = 0; i < original.length; i++) {
		copy_new_str[i] = original.str[i];
	}

	copy->length = original.length;
	copy->str = copy_new_str;
	copy->str[original.length] = '\0';
}

string cat_strings(int num_strings, ...)
{
	string catted_string, * another_string;
	catted_string.length = 0;
	catted_string.str = NULL;
	char* tmp = NULL;

	va_list ptr = NULL;
	va_start(ptr, num_strings);

	int i, j, k;
	size_t total_length = 0;

	if (!(catted_string.str = (char*)malloc(sizeof(char)))) {
		catted_string.length = nan;

		if (catted_string.str) {
			delete_string(&catted_string);
		}

		return catted_string;
	}

	for (i = 0; i < num_strings; i++) {
		another_string = va_arg(ptr, string*);
		total_length += another_string->length;
		if (!(tmp = (char*)realloc(catted_string.str, (total_length + 1) * sizeof(char)))) {
			catted_string.length = nan;

			if (catted_string.str) {
				delete_string(&catted_string);
			}

			return catted_string;
		}
		catted_string.str = tmp;

		for (j = catted_string.length, k = 0; j < total_length; j++, k++) {
			catted_string.str[j] = another_string->str[k];
		}

		catted_string.length = total_length;
	}

	catted_string.str[total_length] = '\0';

	return catted_string;
}

string double_string(string original)
{
	string double_new;

	char* double_str = NULL;
	int i;

	if (!original.length || !original.str) {
		double_new.length = 0;
		double_new.str = NULL;
		return double_new;
	}

	if (!(double_str = (char*)malloc(sizeof(char) * (original.length + 1)))) {
		double_new.length = nan;
		return double_new;
	}

	for (i = 0; i < original.length; i++) {
		double_str[i] = original.str[i];
	}

	double_new.length = original.length;
	double_new.str = double_str;
	double_new.str[original.length] = '\0';

	return double_new;
}

int lexic_comparator(string first, string second)
{
	int i;
	for (i = 0; i < first.length; i++) {
		if (second.length == i || first.str[i] > second.str[i]) {
			return 1;
		}

		if (first.str[i] < second.str[i]) {
			return -1;
		}
	}

	if (second.length > i) {
		return -1;
	}

	return 0;
}