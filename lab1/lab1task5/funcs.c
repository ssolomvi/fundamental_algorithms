#define _CRT_SECURE_NO_WARNINGS
#include <ctype.h>
#include <stdlib.h>
#include "header.h"

fi_statuses fi(char* f_info_name)
{
	FILE* f_info, * f_w, *f_r;
	if (!(f_info = fopen(f_info_name, "r")) || !(f_w = fopen("result.txt", "w"))) {
		return fi_open_error;
	}

	while (!feof(f_info)) {
		char * filename_r, ch, _ch = 0;
		int size = 0;

		if (!(filename_r = (char*)malloc(sizeof(char) * FILENAME_MAX))) {
			fclose(f_info);
			fclose(f_w);
			return fi_allocation_error;
		}

		while (_ch != '\n' && _ch != EOF) {
			ch = fgetc(f_info);

			if (!isspace(ch) && (isspace(_ch) || _ch == 0)) {
				filename_r[size++] = ch;
			}

			else if (!isspace(ch) && !isspace(ch) && ch != EOF) {
				if (size == FILENAME_MAX - 1) {
					fclose(f_info);
					fclose(f_w);
					return fi_incorrect_file_name;
				}
				filename_r[size++] = ch;
			}

			else if (isspace(ch) && !isspace(_ch) && _ch != 0 || ch == EOF) {
				filename_r[size] = '\0';
				break;
			}

			_ch = ch;
		}

		if (ch == '\n') {
			continue;
		}

		if (!(f_r = fopen(filename_r, "r"))) {
			fclose(f_info);
			fclose(f_w);
			return fi_open_error;
		}

		char q;
		while ((q = fgetc(f_r)) != EOF) {
			fputc(q, f_w);
		}

		free(filename_r);

		fputc('\n', f_w);
		fclose(f_r);
	}

	fclose(f_info);
	fclose(f_w);
	return fi_ok;
}

cin_statuses cin()
{
	FILE* f_w, *f_r;
	if (!(f_w = fopen("result.txt", "w"))) {
		return cin_open_error;
	}


	char fileName[FILENAME_MAX], c;
	while (scanf("%s", fileName)) {
		if (!strcmp(fileName, "stop")) {
			break;
		}
		if (!(f_r = fopen(fileName, "r"))) {
			fclose(f_w);
			return cin_open_error;
		}

		while ((c = getc(f_r)) != EOF) {
			fputc(c, f_w);
		}

		fputc('\n', f_w);
		fclose(f_r);
	}

	fclose(f_w);
	return cin_ok;
}

arg_statuses arg(int argc, char** argv)
{
	FILE* f_w, * f_r;
	if (!(f_w = fopen("result.txt", "w"))) {
		return arg_open_error;
	}

	int i;
	for (i = 2; i < argc; i++) {
		if (!(f_r = fopen(argv[i], "r"))) {
			fclose(f_w);
			return arg_open_error;
		}

		char c;
		while ((c = getc(f_r)) != EOF) {
			fputc(c, f_w);
		}

		fputc('\n', f_w);
		fclose(f_r);
	}

	fclose(f_w);
	return arg_ok;
}
