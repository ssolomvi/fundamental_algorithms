#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void help(char*);
void incorrect(char*);

typedef enum read_lex_statuses {
	read_lex_allocation_error,
	read_lex_reallocation_error,
	read_lex_ok
} read_lex_statuses;

read_lex_statuses read_lex(char** lex, int* size, FILE* file_r, char ch);

// TODO add managing errors (opening files, realocation & allocation errors)
int main(int argc, char** argv)
{
	if (argc <= 1) {
		incorrect(*argv);
		return 0;
	}

	if (!strcmp(argv[1], "--help")) {
		help(*argv);
		return 0;
	}

	FILE* file_r, * file_w;

	if (!(file_r = fopen(argv[1], "r")) || !(file_w = fopen("tmp_file", "w"))) {
		printf("Error: file not found or failed to open\n");
		return 0;
	}

	char* lex1 = NULL, * lex2 = NULL, * lex3 = NULL, ch, _ch = 0;
	int i, j, size1, size2, size3;
	read_lex_statuses read_lex_s;

	while (!feof(file_r)) {
		do {
			ch = fgetc(file_r);

			if (!isspace(ch) && (isspace(_ch) || _ch == 0)) {
				// lex starts
				if (lex1 == NULL) {
					read_lex_s = read_lex(&lex1, &size1, file_r, ch);
					int d = sizeof(char) * (size1 - 1);
					switch (read_lex_s)
					{
					case read_lex_allocation_error:
						printf("Allocation error happened\n");
						break;
					case read_lex_reallocation_error:
						printf("Reallocation error happened\n");
						break;
					default:
						ch = ' ';
						break;
					}
				}

				else if (lex2 == NULL) {
					read_lex_s = read_lex(&lex2, &size2, file_r, ch);
					switch (read_lex_s)
					{
					case read_lex_allocation_error:
						printf("Allocation error happened\n");
						break;
					case read_lex_reallocation_error:
						printf("Reallocation error happened\n");
						break;
					default:
						ch = ' ';
						break;
					}
				}

				else if (lex3 == NULL) {
					read_lex_s = read_lex(&lex3, &size3, file_r, ch);
					switch (read_lex_s)
					{
					case read_lex_allocation_error:
						printf("Allocation error happened\n");
						break;
					case read_lex_reallocation_error:
						printf("Reallocation error happened\n");
						break;
					default:
						ch = '\n';
						break;
					}
				}
			}
			_ch = ch;
		} while (ch != '\n');

		if (lex1 != NULL && lex2 != NULL && lex3 != NULL) {
			fputs(lex3, file_w); fputc(' ', file_w);
			fputs(lex1, file_w); fputc(' ', file_w);
			fputs(lex2, file_w); fputc('\n', file_w);

			free(lex1);
			free(lex2);
			free(lex3);
			lex1 = NULL; lex2 = NULL; lex3 = NULL;
		}
	}


	free(lex1);
	free(lex2);
	free(lex3);
	fclose(file_r);
	fclose(file_w);

	if (remove(argv[1]) == -1 || rename("tmp_file", argv[1]) == -1) {
		printf("Error renaming file\n");
	} 

	printf("Programm ended up working successfully!\nThe results are saved in the same file\n");

	return 0;
}

void help(char* name)
{
	printf("The programm operates with file which data is stored in 3 columns\n");
	printf("This means that every string consists of 3 lexemes\n");
	printf("The programm replaces 1st column with 3rd, 2nd one with 1st, 3rd with 2nd\n");
	printf("Syntax: %s [path_to_file]\n\n", name);
}

void incorrect(char* name)
{
	printf("Incorrect syntax\n");
	printf("For help type %s --help", name);
}

read_lex_statuses read_lex(char** lex, int* size, FILE* file_r, char ch)
{
	int LEX_SIZE = 128;
	char* tmp;

	if (!((*lex) = (char*)malloc(sizeof(char) * LEX_SIZE))) {
		return read_lex_allocation_error;
	}

	(*size) = 0;

	while (!isspace(ch) && ch != EOF) {
		if ((*size) == LEX_SIZE - 1) {
			LEX_SIZE *= 2;
			if (!(tmp = (char*)realloc((*lex), LEX_SIZE * sizeof(char)))) {
				free(*lex);
				return read_lex_reallocation_error;
			}
			*lex = tmp;
		}

		(*lex)[(*size)++] = ch;
		ch = fgetc(file_r);
	}

	int mult = (*size) + 1;

	if (!(tmp = (char*)realloc((*lex), mult * sizeof(char)))) {
		free(*lex);
		return read_lex_reallocation_error;
	}
	*lex = tmp;
	(*lex)[(*size)] = '\0';

	return read_lex_ok;
}
