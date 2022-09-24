#include <stdio.h>
#include "header5.h"

void incorrect(char*);
void help();

int main(int argc, char** argv)
{
	if (argc <= 1) {
		incorrect(*argv);
		return 0;
	}

	if (!strcmp(argv[1], "--help")) {
		help(argv[0]);
	}

	else if (!strcmp("-fi", argv[1])) {
		if (argc != 3) {
			incorrect(*argv);
			return 0;
		}

		fi_statuses fi_s = fi(argv[2]);
		switch (fi_s)
		{
		case fi_incorrect_file_name:
			printf("Entered file name is too large\n");
			break;
		case fi_open_error:
			printf("Open file error\nCheck if the file exists or if you have rights to open this file\n");
			break;
		default:
			printf("Files concatenated successfully!\n");
			break;
		}
	}

	else if (!strcmp("-cin", argv[1])) {
		if (argc > 2) {
			incorrect(*argv);
			return 0;
		}

		cin_statuses cin_s = cin();
		switch (cin_s)
		{
		case cin_open_error:
			printf("Open file error\nCheck if the file exists or if you have rights to open this file\n");
			break;
		default:
			printf("Files concatenated successfully!\n");
			break;
		}

	}
	
	else if (!strcmp("-arg", argv[1])) {
		arg_statuses arg_s = arg(argc, argv);
		switch (arg_s)
		{
		case arg_open_error:
			printf("Open file error\nCheck if the file exists or if you have rights to open this file\n");
			break;
		default:
			printf("Files concatenated successfully!\n");
			break;
		}
	}

	return 0;
}

void incorrect(char* argv)
{
	printf("Incorrect syntax\n");
	printf("Type %s --help for help\n", argv);
}

void help()
{
	printf("The programm concatenates files in one file.\nResult is saved in file result.txt.\n Add a flag:\n");
	printf("\t-fi <filename> .txt file <filename> contains information \n");
	printf("\t               of list of names of files with input data\n\n");
	printf("\t-cin           the list of names of files are read from stdin; use 'stop' to stop entering\n\n");
	printf("\t-arg           the list of names of files are passed through command line arguments\n");
}
