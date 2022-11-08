#include "header5.h"
#define VALID_USER_INPUT(x) ((x) == '1' || (x) == '2' || (x) == '3' || (x) == '4' || (x) == '5')


void help();

int main(int argc, char** argv)
{
	if (argc != 2) {
		printf("Incorrect count of command line arguments\n");
		return -1;
	}

	FILE* in = NULL;
	if (!(in = fopen(argv[1], "r"))) {
		printf("Error opening file\n");
		return -2;
	}

	student* students = NULL;
	int count = 0;
	read_students_statuses read_students_s = read_students(&students, &count, in);
	switch (read_students_s)
	{
	case read_students_incorrect_ptr_to_array:
		printf("Incorrect ptr to an dynamic array of students passed. It must be NULL\n");
		fclose(in);
		return -3;
	case read_students_malloc_error:
		printf("Memory allocation error happened\n");
		fclose(in);
		return -4;
	case read_students_realloc_error:
		printf("Memory reallocation error happened\n");
		fclose(in);
		return -5;
	case read_students_invalid_data:
		printf("Invalid data passed in file %s\n", argv[1]);
		fclose(in);
		return -6;
	default:
		printf("An array was successfully read from file!\n");
		break;
	}

	char user_input[BUFSIZ] = {0};
	char user_input_flag[BUFSIZ] = {0};
	int exited = 0;
	student* collection = NULL;
	size_t collection_num = 0;

	find_statuses find_statuses_s;
	char user_stream[BUFSIZ];
	FILE* f_stream;

	above_average_statuses above_average_statuses_s;

	student to_find;

	help();

	while (!exited)
	{
		printf(">>> ");
		scanf("%s", user_input);
		if (strlen(user_input) != 1 || !VALID_USER_INPUT(*user_input)) {
			printf("Invalid input! Try again...\n\n");
			continue;
		}

		switch (*user_input - 48)
		{
		case 1:
			printf("You have chosen find command. Enter flag:\n>>> ");
			scanf("%s", user_input_flag);
			{
				if (!strcmp(user_input_flag, "-i")) {
					printf("Enter student id to find:\n>>> ");
					scanf("%u", &(to_find.id));
					find_statuses_s = find(&to_find, students, count, &collection, &collection_num, comp_id);
				}
				else if (!strcmp(user_input_flag, "-s")) {
					printf("Enter student(s) surname to find:\n>>> ");
					scanf("%s", to_find.surname);
					find_statuses_s = find(&to_find, students, count, &collection, &collection_num, comp_surname);
				}
				else if (!strcmp(user_input_flag, "-n")) {
					printf("Enter student(s) name to find:\n>>> ");
					scanf("%s", to_find.name);
					find_statuses_s = find(&to_find, students, count, &collection, &collection_num, comp_name);
				}
				else if (!strcmp(user_input_flag, "-g")) {
					printf("Enter student(s) group to find:\n>>> ");
					scanf("%s", to_find.group);
					find_statuses_s = find(&to_find, students, count, &collection, &collection_num, comp_group);
				}
				else if (!strcmp(user_input_flag, "-c")) {
					printf("Enter student(s) course to find:\n>>> ");
					scanf("%hu", &(to_find.course));
					find_statuses_s = find(&to_find, students, count, &collection, &collection_num, comp_course);
				}
				else {
					printf("Incorrect flag! Try again\n");
					continue;
				}

				switch (find_statuses_s)
				{
				case find_incorrect_ptr_to_collection:
					printf("Incorrcet ptr to collection passed in function find\n");
					clear(&students, &count);
					fclose(in);
					return -7;
				case find_incorrect_ptr_to_first:
					printf("Incorrect ptr to students array passed in function find\n");
					fclose(in);
					clear(&students, &count);
					return -8;
				case find_malloc_error:
					printf("Memory allocation error happened in function find\n");
					fclose(in);
					clear(&students, &count);
					return -9;
				case find_realloc_error:
					printf("Memory reallocation error happened in function find\n");
					fclose(in);
					clear(&students, &count);
					return -10;
				case find_no_coincidence:
					printf("There are no found students with such data\n");
					free(collection);
					collection_num = 0;
					continue;
				default:
					printf("Enter stream to print found students collection:\n>>> ");
					scanf("%s", user_stream);
					if (!strcmp("stdout", user_stream)) {
						print_students(stdout, collection, collection_num);
					}
					else {
						if (!(f_stream = fopen(user_stream, "w"))) {
							printf("Error opening file. Try again\n");
							collection_num = 0;
							free(collection);
							continue;
						}
						print_students(f_stream, collection, collection_num);
						fclose(f_stream);
					}
					collection_num = 0;
					free(collection);
					break;
				}
			}
			break;
		case 2:
			printf("You have chosen sort command. Enter flag:\n>>> ");
			scanf("%s", user_input_flag);
			{
				if (!strcmp(user_input_flag, "-i")) {
					qsort(students, count, sizeof(student), comp_id);
				}
				else if (!strcmp(user_input_flag, "-s")) {
					qsort(students, count, sizeof(student), comp_surname);
				}
				else if (!strcmp(user_input_flag, "-n")) {
					qsort(students, count, sizeof(student), comp_name);
				}
				else if (!strcmp(user_input_flag, "-g")) {
					qsort(students, count, sizeof(student), comp_group);
				}
				else if (!strcmp(user_input_flag, "-c")) {
					qsort(students, count, sizeof(student), comp_course);
				}
				else {
					printf("Incorrect flag! Try again\n");
					continue;
				}

				printf("An array was sorted successfully!\n");
				printf("If you wish to print sorted students array enter stream; else print \"no\":\n>>> ");
				scanf("%s", user_stream);
				if (!strcmp("no", user_stream)) {
					continue;
				}
				if (!strcmp("stdout", user_stream)) {
					print_students(stdout, students, count);
				}
				else {
					if (!(f_stream = fopen(user_stream, "w"))) {
						printf("Error opening file. Try again\n");
						continue;
					}
					print_students(f_stream, students, count);
					fclose(f_stream);
				}
			}
			break;
		case 3:
			printf("Enter stream to print students data array:\n>>> ");
			scanf("%s", user_stream);
			if (!strcmp("stdout", user_stream)) {
				print_students(stdout, students, count);
			}
			else {
				if (!(f_stream = fopen(user_stream, "w"))) {
					printf("Error opening file. Try again\n");
					continue;
				}
				print_students(f_stream, students, count);
				fclose(f_stream);
			}
			break;
		case 4:
			above_average_statuses_s = above_average(argv[1], students, count);
			switch (above_average_statuses_s)
			{
			case above_average_open_file_error:
				printf("Open file error\n");
				fclose(in);
				clear(&students, &count);
				return -11;
			case above_average_malloc_error:
				printf("Memory allocation error happened\n");
				fclose(in);
				clear(&students, &count);
				return -12;
			case above_average_incorrect_ptr_to_collection:
				printf("Incorrect ptr to collection passed!\n");
				fclose(in);
				clear(&students, &count);
				return -13;
			case above_average_incorrect_ptr_to_first:
				printf("Incorrect ptr to students data array passed!\n");
				fclose(in);
				clear(&students, &count);
				return -14;
			case above_average_realloc_error:
				printf("Memory realloaction error happened\n");
				fclose(in);
				clear(&students, &count);
				return -15;
			default:
				printf("Students' data with assessment above average were put in trace files\n");
				break;
			}
			break;
		case 5:
			exited++;
			break;
		}
	}

	clear(&students, &count);
	fclose(in);
	return 0;
}

void help()
{
	printf("The programm works with list of students wrote down in file passed as command line argument.\n");
	printf("\t1 - find student(s) data and print found ones in stream. Flags:\n");
	printf("\t\t-i by id\n\t\t-s by surname\n\t\t-n by name\n\t\t-g by group\n\t\t-c by course\n");
	printf("\t2 - sort students list. Flags:\n");
	printf("\t\t-i by id\n\t\t-s by surname\n\t\t-n by name\n\t\t-g by group\n\t\t-c by course\n");
	printf("\t3 - print students list in stream\n");
	printf("\t4 - output to trace files of format {command line argument}{course number}\n");
	printf("\t    prints students with assessments higher than average\n");
	printf("\t5 - exit\n");
	printf("You choose:\n");
}

void foo(int (*callback)(int a, char b))
{

}