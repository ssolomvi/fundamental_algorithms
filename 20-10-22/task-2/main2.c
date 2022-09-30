#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void help(char* argv);
void reverse(char** string);
void uppercase(char** string);
void sort_str(char** string);
char* my_str_cat(char* str1, char* str2);

int main(int argc, char** argv)
{
	if (argc <= 1) {
		printf("Incorrect syntax. Type %s --help for help\n", *argv);
		return 0;
	}

	if (!strcmp(argv[1], "--help")) {
		help(*argv);
		return 0;
	}

	if (argc < 3) {
		printf("Incorrect syntax. Type %s --help for help\n", *argv);
		return 0;
	}

	if (!strcmp(argv[2], "-l")) {
		printf("The length of the entered string: %d\n", (int)strlen(argv[1]));
	}

	else if (!strcmp(argv[2], "-r")) {
		reverse(&argv[1]);
		printf("The reversed string: %s\n", argv[1]);
	}

	else if (!strcmp(argv[2], "-u")) {
		uppercase(&argv[1]);
		printf("The string with every letter in odd position uppercase: %s\n", argv[1]);
	}

	else if (!strcmp(argv[2], "-n")) {
		sort_str(&argv[1]);
		if (!argv[1]) {
			printf("Memory allocation error happened\n");
			return 0;
		}
		printf("The sorted string: %s", argv[1]);
	}

	else if (!strcmp(argv[2], "-c")) {
		if (argc != 4) {
			printf("Incorrect syntax. Type %s --help for help\n", *argv);
			return 0;
		}
		char* new_str = my_str_cat(argv[1], argv[3]);
		if (!new_str) {
			printf("Memory allocation error happened\n");
			return 0;
		}
		printf("The concatenated string: %s\n", new_str);
		free(new_str);
	}

	else {
		printf("Incorrect syntax. Type %s --help for help\n", *argv);
	}

	return 0;
}

void help(char* argv)
{
	printf("The programm works with string\n");
	printf("Syntax:\n\t%s [string1] [flag] (optional)<string2>\n\n", argv);
	printf("Flags you can use:\n");
	printf("\t-l  counts length of the string\n");
	printf("\t-r  reverses the string\n");
	printf("\t-u  makes every element in odd position uppercase\n");
	printf("\t-n  makes the string like: \"[digits][letters][other symbols]\" saving the order\n");
	printf("\t-c  concatenates string1 with string2 (3rd argument is neccessary)\n");
}

void reverse(char** string)
{
	int i, j;
	int str_len = strlen((*string));
	char tmp;
	for (i = 0, j = str_len - 1; i < str_len / 2; j--, i++) {
		tmp = (*string)[i];
		(*string)[i] = (*string)[j];
		(*string)[j] = tmp;
	}
}

void uppercase(char** string)
{
	int i, str_len = strlen((*string));

	for (i = 1; i < str_len; i += 2) {
		if ((*string)[i] >= 'A' && (*string)[i] <= 'Z') {
			(*string)[i] += ' ';
		}
	}
}

void sort_str(char** string)
{
	int i, str_len = strlen(*string), digits = 0, letters = 0, other = 0;
	
	for (i = 0; i < str_len; i++) {
		if (isdigit((*string)[i])) {
			digits++;
		}
		else if (isalpha((*string)[i])) {
			letters++;
		}
		else {
			other++;
		}
	}
	
	char* copy_str = (char*)malloc(sizeof(char) * (str_len + 1));
	if (!copy_str) {
		(*string) = NULL;
	}

	strcpy(copy_str, (*string));
	int digits_curr_ind = 0, letters_curr_ind = digits, other_curr_ind = digits + letters;

	for (i = 0; i < str_len; i++) {
		if (isdigit(copy_str[i])) {
			(*string)[digits_curr_ind++] = copy_str[i];
		}

		else if (isalpha(copy_str[i])) {
			(*string)[letters_curr_ind++] = copy_str[i];
		}

		else {
			(*string)[other_curr_ind++] = copy_str[i];
		}
	}

	free(copy_str);
}

char* my_str_cat(char* str1, char* str2)
{
	char* str_copy;
	int str1_len = strlen(str1);
	if (!(str_copy = (char*)malloc(sizeof(char) * (str1_len + strlen(str2) + 1)))) {
		return NULL;
	}
	strcpy(str_copy, str1);
	strcpy(str_copy + str1_len, str2);
	return str_copy;
}