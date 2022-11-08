#include "header7.h"
#define VALID_USER_INPUT(x) ((x) == '1' || (x) == '2' || (x) == '3' || (x) == '4' || (x) == '5')

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
	list_cit* my_list = NULL;
	read_citizens_statuses status = read_citizens(in, &my_list);
	fclose(in);

	switch (status)
	{
	case read_citizens_incorrect_ptr_to_file:
		printf("Incorrect ptr to file passed in function read_citizens!\n");
		return -3;
	case read_citizens_incorrect_ptr_to_list:
		printf("Incorrect ptr to list passed in function read_citizens!\n");
		return -4;
	case read_citizens_incorrect_ptr_to_value:
		printf("Incorrect ptr to data passed in function copy_citizen!\n");
		return -5;
	case read_citizens_incorrect_data:
		printf("Incorrect data / data format passed in file!\n");
		return -6;
	case read_citizens_malloc_error:
		printf("Memory allocation error happened!\n");
		return -7;
	case read_citizens_realloc_error:
		printf("Memory reallocation error happened!\n");
		return -8;
	default:
		printf("The information about citizens was successfully read from file %s!\n", argv[1]);
		break;
	}

	int exited = 0, respond = 0;
	char user_input[BUFSIZ], user_stream[FILENAME_MAX];

	citizen* tmp_citizen = NULL, * found = NULL;

	while (!exited)
	{
		printf(">>> ");
		if (scanf("%s", user_input) != 1) {
			printf("Error reading you input!\n");
			delete_citizens(&my_list);
			return -9;
		}
		if (strlen(user_input) != 1 && !VALID_USER_INPUT(*user_input)) {
			printf("Invalid input! Try again...\n\n");
			continue;
		}

		switch (*user_input - '0')
		{
		case 1:
			printf("Enter stream from where you want to add citizens info to list:\n>>> ");
			if (scanf("%s", user_stream) != 1) {
				printf("Error reading you input!\n");
				delete_citizens(&my_list);
				return -10;
			}

			if (!strcmp("stdin", user_stream)) {
				printf("Enter citizens' data below. Remember it must be in correct format!\n");
				status = read_citizens(stdin, &my_list);
			}
			else {
				if (!(in = fopen(user_stream, "r"))) {
					printf("Error opening file specified! Try again!\n");
					continue;
				}
				status = read_citizens(in, &my_list);
				fclose(in);
			}

			switch (status)
			{
			case read_citizens_incorrect_ptr_to_file:
				printf("Incorrect ptr to file passed in function read_citizens!\n");
				delete_citizens(&my_list);
				return -11;
			case read_citizens_incorrect_ptr_to_list:
				printf("Incorrect ptr to list passed in function read_citizens!\n");
				delete_citizens(&my_list);
				return -12;
			case read_citizens_incorrect_ptr_to_value:
				printf("Incorrect ptr to data passed in function copy_citizen!\n");
				delete_citizens(&my_list);
				return -13;
			case read_citizens_incorrect_data:
				printf("Incorrect data / data format passed in file!\n");
				delete_citizens(&my_list);
				return -14;
			case read_citizens_malloc_error:
				printf("Memory allocation error happened!\n");
				delete_citizens(&my_list);
				return -15;
			case read_citizens_realloc_error:
				printf("Memory reallocation error happened!\n");
				delete_citizens(&my_list);
				return -16;
			default:
				printf("The information about citizens was successfully read from file %s!\n", user_stream);
				break;
			}
			break;
		case 2:
			tmp_citizen = (citizen*)malloc(sizeof(citizen));
			if (!tmp_citizen) {
				printf("Memory allocation error happened!\n");
				delete_citizens(&my_list);
				return -17;
			}
			printf("Enter Surname, Name and Patronymic of person you want to delete from list:\n");
			if ((status = read_string(stdin, &(tmp_citizen->surname), ' ') != read_citizens_ok) ||
				(status = read_string(stdin, &(tmp_citizen->name), ' ') != read_citizens_ok) ||
				(status = read_string(stdin, &(tmp_citizen->patronymic), '\n') != read_citizens_ok))
			{
				free_multi(4, tmp_citizen->surname, tmp_citizen->name, tmp_citizen->patronymic, tmp_citizen);
				delete_citizens(&my_list);
				return status;
			}
			respond = delete_n_find_citizen(&my_list, tmp_citizen, lexic_compare_snp);
			if (!respond) {
				printf("There is no citizen with such data in the list!\n");
			}
			else {
				printf("The citizen %s %s %s was deleted from the list!\n", tmp_citizen->surname, tmp_citizen->name, tmp_citizen->patronymic);
			}
			free_multi(4, tmp_citizen->surname, tmp_citizen->name, tmp_citizen->patronymic, tmp_citizen);
			break;
		case 3:
			tmp_citizen = (citizen*)malloc(sizeof(citizen));
			if (!tmp_citizen) {
				printf("Memory allocation error happened!\n");
				delete_citizens(&my_list);
				return -17;
			}
			printf("Enter Surname, Name and Patronymic of person you want to find in the list:\n");
			if ((status = read_string(stdin, &(tmp_citizen->surname), ' ') != read_citizens_ok) ||
				(status = read_string(stdin, &(tmp_citizen->name), ' ') != read_citizens_ok) ||
				(status = read_string(stdin, &(tmp_citizen->patronymic), '\n') != read_citizens_ok))
			{
				free_multi(4, tmp_citizen->surname, tmp_citizen->name, tmp_citizen->patronymic, tmp_citizen);
				delete_citizens(&my_list);
				return status;
			}

			found = search(my_list, tmp_citizen, lexic_compare_snp);
			if (!found) {
				printf("Specified citizen not found or empty list passed!\n");
			}
			else {
				printf("The found citizen:\n");
				print_citizen(stdout, found);
			}
			found = NULL;
			free_multi(4, tmp_citizen->surname, tmp_citizen->name, tmp_citizen->patronymic, tmp_citizen);
			break;
		case 4:
			printf("Enter stream where you would like to put citizens' info:\n>>> ");
			if (scanf("%s", user_stream) != 1) {
				printf("Error reading you input!\n");
				delete_citizens(&my_list);
				return -18;
			}

			if (!strcmp("stdout", user_stream)) {
				print_list(stdout, my_list);
			}
			else {
				if (!(out = fopen(user_stream, "w"))) {
					printf("Error opening file specified! Try again!\n");
					continue;
				}
				print_list(out, my_list);
				fclose(out);
			}
			break;
		case 5:
			exited++;
			break;
		}
	}


	delete_citizens(&my_list);
	return 0;
 }