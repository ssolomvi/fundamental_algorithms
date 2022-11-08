#ifndef HEADER_7

#define HEADER_7

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum sex {
	male = 1,
	female = 2
} sex;

typedef struct date {
	unsigned year;
	unsigned month;
	unsigned day;
} date;

typedef struct citizen {
	char* surname;
	char* name;
	char* patronymic;
	date birthday;
	sex sex;
	float average_income;
} citizen;

typedef struct list_cit {
	citizen* data;
	struct list_cit* next;
} list_cit;

typedef enum read_citizens_statuses {
	read_citizens_incorrect_ptr_to_file,
	read_citizens_incorrect_ptr_to_list,
	read_citizens_incorrect_ptr_to_value,
	read_citizens_incorrect_data,
	read_citizens_malloc_error,
	read_citizens_realloc_error,
	read_citizens_ok
} read_citizens_statuses;

void delete_citizens(list_cit** to_del);
read_citizens_statuses read_citizens(FILE* input, list_cit** head);
void print_list(FILE* stream, list_cit* to_print);

#endif // !HEADER_7