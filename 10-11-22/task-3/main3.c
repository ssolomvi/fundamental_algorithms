<<<<<<< HEAD
#include "header3.h"

int main(int argc, char** argv)
{
	if (argc != 4) {
		printf("Incorrect count of command line arguments\n");
		printf("Use --help for help\n");
		return -1;
	}

	FILE* in, *out;
	if (!(in = fopen(argv[1], "r")) || !(out = fopen(argv[3], "w"))) {
		printf("File(s) not opened\n");
		return -2;
	}

	read_employees_statuses read_employees_s = -1;
	int count = 0;
	employee* employee_arr = read_employees(in, &count, &read_employees_s);
	switch (read_employees_s)
	{
	case read_employees_file_error:
		printf("Error opening file\n");
		fclose(in);
		fclose(out);
		return -3;
	case read_employees_malloc_error:
		fclose(in);
		fclose(out);
		printf("Error allocationg memory for an array of employees\n");
		return -4;
	case read_employees_realloc_error:
		fclose(in);
		fclose(out);
		printf("Error reallocating memory for an array of employees\n");
		return -5;
	default:
		printf("An array of employees was read successfully!\nRead employees:\n");
		print_employees(stdout, employee_arr, count);
		break;
	}

	fibonacci_heap_sort_statuses fib_heap_sort_s = -1;
	if (!strcmp(argv[2], "-a") || !strcmp(argv[2], "/a")) {
		fib_heap_sort_s = fibonacci_heap_sort('>', employee_arr, count, out);
		printf("The array of employees was sorted ascending. The result was put in file %s\n", argv[3]);
	}

	else if (!strcmp(argv[2], "-d") || !strcmp(argv[2], "/d")) {
		fib_heap_sort_s = fibonacci_heap_sort('<', employee_arr, count, out);
		printf("The array of employees was sorted descending. The result was put in file %s\n", argv[3]);
	}

	else {
		printf("Incorrect flag in commang line arguments!\n");
		return -6;
	}

	fclose(in);
	fclose(out);
	free(employee_arr);
	return 0;
}
=======
#include "header3.h"

int main(int argc, char** argv)
{
	if (argc != 4) {
		printf("Incorrect count of command line arguments\n");
		printf("Use --help for help\n");
		return -1;
	}

	FILE* in, *out;
	if (!(in = fopen(argv[1], "r")) || !(out = fopen(argv[3], "w"))) {
		printf("File(s) not opened\n");
		return -2;
	}

	read_employees_statuses read_employees_s = -1;
	int count = 0;
	employee* employee_arr = read_employees(in, &count, &read_employees_s);
	switch (read_employees_s)
	{
	case read_employees_file_error:
		printf("Error opening file\n");
		fclose(in);
		fclose(out);
		return -3;
	case read_employees_malloc_error:
		fclose(in);
		fclose(out);
		printf("Error allocationg memory for an array of employees\n");
		return -4;
	case read_employees_realloc_error:
		fclose(in);
		fclose(out);
		printf("Error reallocting memory for an array of employees\n");
		return -5;
	default:
		printf("An array of employees was read successfully!\nRead employees:\n");
		print_employees(stdout, employee_arr, count);
		break;
	}

	fibonacci_heap_sort_statuses fib_heap_sort_s = -1;
	if (!strcmp(argv[2], "-a") || !strcmp(argv[2], "/a")) {
		fib_heap_sort_s = fibonacci_heap_sort('>', employee_arr, count, out);
		printf("The array of employees was sorted ascending. The result was put in file %s\n", argv[3]);
	}

	else if (!strcmp(argv[2], "-d") || !strcmp(argv[2], "/d")) {
		fib_heap_sort_s = fibonacci_heap_sort('<', employee_arr, count, out);
		printf("The array of employees was sorted descending. The result was put in file %s\n", argv[3]);
	}

	else {
		printf("Incorrect flag in commang line arguments!\n");
		return -6;
	}

	fclose(in);
	fclose(out);
	free(employee_arr);
	return 0;
}
>>>>>>> origin/main
