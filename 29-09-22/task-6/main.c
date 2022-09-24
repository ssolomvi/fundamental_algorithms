#include "header6.h"

void help(char* argv);

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

	FILE* f_in, * f_out;
	if (!(f_in = fopen(argv[1], "r")) || !(f_out = fopen("result.txt", "w"))) {
		printf("Open file error\n");
		return 0;
	}

	long double number_10_ld;
	long number_10_l;
	char* number_x = NULL, *number_10_str = NULL, notat_str = NULL;
	int is_double, notat;
	read_number_statuses read_number_s;

	while (!feof(f_in)) {
		read_number_s = read_number(&number_x, f_in, &is_double);
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

		if (is_double) {
			fnotation(number_x, &notat, &number_10_ld);
			number_10_str = fmy_itoa(number_10_ld);
		}
		else {
			notation(number_x, &notat, &number_10_l);
			number_10_str = my_itoa(number_10_l);
		}

		if (!(notat_str = my_itoa(notat))) {
			free(number_x);
			free(number_10_str);
			printf("Allocation error happened\n");
			return 0;
		}

		fputs(number_x, f_out); fputc(" ", f_out);
		fputs(notat_str, f_out); fputc(" ", f_out);
		fputs(number_10_str, f_out);
		fputc('\n', f_out);

		free(number_x);
		free(number_10_str);
	}

	fclose(f_in);
	fclose(f_out);
	return 0;
}

void help(char* argv) {
	printf("The programm reads a file which contains numbers, defines min notation for them\n");
	printf("and converts them in decimal notation.Results are stored in result.txt file.\n\n");
	printf("Syntax: % s <filename>\n", argv);
}
