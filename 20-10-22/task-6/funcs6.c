#include "header6.h"


void read_substring(char** substring)
{
	char lexeme[128] = {0}, *tmp = NULL;
	int lexeme_len = 0, lexeme_count = 0;
	(*substring) = NULL;
	int substring_len = 0;

	if (!((*substring) = (char*)malloc(sizeof(char) * 16))) {
		(*substring) = NULL;
		return;
	}

	printf("Enter substring to search for, use \"stop\" to stop entering:\n");

	while (scanf("%s", lexeme)) {
		if (!strcmp(lexeme, "stop")) {
			break;
		}
		lexeme_len = strlen(lexeme);

		if (!(tmp = (char*)realloc((*substring), sizeof(char) * (lexeme_len + substring_len + lexeme_count + 1)))) {
			free(*substring);
			(*substring) = NULL;
			return;
		}

		(*substring) = tmp;

		strcpy((*substring) + substring_len + lexeme_count, lexeme);
		strcat((*substring), " ");
		substring_len += lexeme_len;
		lexeme_count++;
	}
	(*substring)[substring_len + lexeme_count - 1] = '\0'; // realloc to precise size
	if (!(tmp = (char*)realloc((*substring), sizeof(char) * (substring_len + lexeme_count)))) {
		free(*substring);
		(*substring) = NULL;
	}
	(*substring) = tmp;
}

find_substr_statuses find_substr_in_file(char* substr, int substr_len, found** found_substrs, int* found_size, int* found_count, char* filename)
{
	int j = 0, str_num = 1, char_num = 0;
	char ch;
	FILE* file;
	found* tmp = NULL;


	if (!(file = fopen(filename, "r"))) {
		return find_substr_open_file_error;
	}

	while (!feof(file)) {
		if ((ch = fgetc(file)) == '\n') {
			str_num++;
			char_num = 0;
			continue;
		}
		char_num++;


		if (ch == substr[j]) {
			for (++j; j < substr_len; j++) {
				ch = fgetc(file);
				if (ch != substr[j]) {
					if (ch == '\n') {
						str_num++;
					}
					j = 0;
					break;
				}
			}
			if (ch == substr[j - 1] || ch == EOF) {
				if ((*found_count) >= (*found_size)-1) {
					(*found_size) *= 2;
					if (!(tmp = (found*)realloc((*found_substrs), sizeof(found) * (*found_size)))) {
						return find_substr_realloc_error;
					}
					(*found_substrs) = tmp;
				}
				(*found_substrs)[(*found_count)].file_name = filename;
				(*found_substrs)[(*found_count)].string_num = str_num;
				(*found_substrs)[(*found_count)++].symbol_num = char_num;
				j = 0;
				char_num += substr_len - 1;
			}
		}
	}
	
	fclose(file);
}

find_substr_statuses find_substr(char* substr, found** found_substrs, int* found_count, int count, ...)
{
	int i, substr_len = strlen(substr), found_size = 2;
	found* tmp = NULL;
	find_substr_statuses find_substr_s;

	if (!((*found_substrs) = (found*)malloc(sizeof(found) * 4))) {
		return find_substr_malloc_error;
	}

	va_list ptr;
	va_start(ptr, count);

	for (i = 0; i < count; i++) {
		find_substr_s = find_substr_in_file(substr, substr_len, found_substrs, &found_size, found_count, va_arg(ptr, char*));
		switch (find_substr_s)
		{
		case find_substr_realloc_error:
			return find_substr_realloc_error;
		case find_substr_open_file_error:
			return find_substr_open_file_error;
		default:
			break;
		}
	}
	
	va_end(ptr);


	if (*found_count != 0) {
		if (!(tmp = (found*)realloc((*found_substrs), sizeof(found) * (*found_count)))) {
			return find_substr_realloc_error;
		}
	}
	

	return find_substr_ok;
}

void print_found(found* found_substrs, int found_count)
{
	if (found_count == 0) {
		printf("There is no found substring in entered files\n");
		return;
	}

	int i;
	
	for (i = 0; i < found_count; i++) {
		printf("The %d found substring in file %s:\n", i + 1, found_substrs[i].file_name);
		printf("Starts with string number %d, symbol number %d\n\n", found_substrs[i].string_num, found_substrs[i].symbol_num);
	}
}