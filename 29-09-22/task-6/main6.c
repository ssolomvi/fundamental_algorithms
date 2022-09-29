#define _CRT_SECURE_NO_WARNINGS
#include "header6.h"

void help(char*);

int main(int argc, char** argv)
{
	if (argc != 2) {
		printf("Invalid syntax\nFor help type %s --help", *argv);
		return 0;
	}

	if (!strcmp(argv[1], "--help")) {
		help(*argv);
		return 0;
	}

	FILE* f_in = NULL, * f_out = NULL;

	if (!(f_in = fopen(argv[1], "r")) || !(f_out = fopen("result.txt", "w"))) {
		printf("Open file error!\n");
		return 0;
	}
	
	char* number_x = NULL, *number_10 = NULL, *notation_str = NULL;
	int notation;
	long long num;
	read_number_statuses read_number_s;
	my_itoa_statuses my_itoa_s;

	while (!feof(f_in)) {
		read_number_s = read_number(&number_x, f_in);
		switch (read_number_s)
		{
		case read_number_alloc_error:
			printf("Allocation error happened\n");
			return 0;
		case read_number_realloc_error:
			printf("Reallocation error happened\n");
			return 0;
		case read_number_incorrect_number:
			printf("The enetered number is incorrect\n");
			return 0;
		default:
			break;
		}

		notation = get_notation(number_x);
		if (notation == 0) {
			printf("Entered data error!\n");
			return 0;
		}
		my_itoa_s = my_itoa(notation, &notation_str);
		if (my_itoa_s == my_itoa_malloc_error || my_itoa_s == my_itoa_realloc_error) {
			free(number_x);
			fclose(f_in);
			fclose(f_out);
			printf("Memory error\n");
			return 0;
		}

		num = to_decimal(number_x, notation);
		my_itoa_s = my_itoa(num, &number_10);
		if (my_itoa_s == my_itoa_malloc_error || my_itoa_s == my_itoa_realloc_error) {
			free(number_x);
			free(notation_str);
			fclose(f_in);
			fclose(f_out);
			printf("Memory error\n");
			return 0;
		}

		fputs(number_x, f_out); fputc(' ', f_out);
		fputs(notation_str, f_out); fputc(' ', f_out);
		fputs(number_10, f_out); fputc('\n', f_out);
		free(number_x);
		free(number_10);
		free(notation_str);
	}


	fclose(f_in);
	fclose(f_out);

	printf("The results of programm are saved in result.txt\n");

	return 0;
}

void help(char* argv) {
	printf("The programm reads a file which contains numbers, defines min notation for them\n");
	printf("and converts them in decimal notation.Results are stored in result.txt file.\n\n");
	printf("Syntax: % s <filename>\n", argv);
}