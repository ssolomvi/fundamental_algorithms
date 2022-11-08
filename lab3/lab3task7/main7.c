#include "header7.h"

int main(int argc, char** argv)
{
	if (argc != 2) {
		printf("Pass the file path only\n");
		return -1;
	}

	FILE* in = NULL, * out = NULL;

	if (!(in = fopen(argv[1], "r"))) {
		printf("Error opening file!\n");
		return -2;
	}

	list_cit* my_list = (list_cit*)malloc(sizeof(list_cit));
	if (!my_list) {
		return -1;
	}
	my_list = &(list_cit){ .next = NULL };
	// my_list->data = &(citizen) { .average_income = 0, .name = NULL };
	read_citizens_statuses status = read_citizens(in, my_list);
	/*
	switch (status)
	{
	case read_citizens_incorrect_ptr_to_file:
		printf("Incorrect ptr to file passsed!\n");
		return -1;
	case read_citizens_incorrect_ptr_to_list:
		printf("Incorrect ptr to list passsed!\n");
		return -1;
	case read_citizens_incorrect_ptr_to_value:
		printf("Incorrect ptr to value passsed!\n");
		return -1;
	case read_citizens_incorrect_data:
		printf("Incorrect data in file passsed!\n");
		return -1;
	case read_citizens_malloc_error:
		printf("Memory allocation error happened\n");
		return -1;
	default:
		printf("Your list was successfully read!\n");
		break;
	}
	*/
	print_list(stdout, my_list);
	int a = 5;
	fclose(in);

	if (!(in = fopen("2.txt", "r"))) {
		printf("Error opening file!\n");
		return -2;
	}
	status = read_citizens(in, my_list);


	print_list(stdout, my_list);
	delete_citizens(&my_list);

	fclose(in);
	return 0;
}