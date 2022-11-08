#include "header5.h"

void clear(student** array, int* allocated)
{   // allocated == last ind
	int i;
	for (i = 0; i < (*allocated); i++) {
		free((*array)[i].assessments);
	}
	free(*array);
	(*allocated) = 0;
	(*array) = NULL;
}

int is_valid_str(char* str)
{
	int str_len = strlen(str), i;
	for (i = 0; i < str_len; i++) {
		if (!isalpha(str[i])) {
			return 0;
		}
	}
	return 1;
}

int is_valid(student to_valid, unsigned int* ids, int count)
{
	if (!is_valid_str(to_valid.name) || !is_valid_str(to_valid.surname)) {
		return 0;
	}

	if (to_valid.course < 1 || to_valid.course > 4) {
		return 0;
	}

	int i;
	for (i = 0; i < count; i++) {
		if (ids[i] == to_valid.id) {
			return 0;
		}
	}

	short assessment = 0;
	for (i = 0; i < 5; i++) {
		assessment = to_valid.assessments[i];
		if (assessment < 2 || assessment > 5) {
			return 0;
		}
	}
}

read_students_statuses read_students(student** students, int* actual_size, FILE* in)
{
	int size = 16;
	student* tmp = NULL;
	(*actual_size) = 0;

	if (!(*students)) { // students array is empty
		if (!((*students) = (student*)malloc(size * sizeof(student)))) {
			return read_students_malloc_error;
		}
	}
	else {
		return read_students_incorrect_ptr_to_array;
	}
	
	unsigned int *ids = (unsigned int*)malloc(sizeof(unsigned int) * size), tmp_ids = NULL;
	int i = 0;
	
	if (!ids) {
		free(*students);
		return read_students_malloc_error;
	}

	while (!feof(in)) {
		if ((*actual_size) >= size - 1) {
			size *= 2;
			if (!(tmp = (student*)realloc((*students), sizeof(student) * size))) {
				clear(students, actual_size);
				free(ids);
				return read_students_realloc_error;
			}
			(*students) = tmp;

			if (!(tmp_ids = (unsigned int*)realloc(ids, sizeof(unsigned int) * size))) {
				clear(students, actual_size);
				free(ids);
				return read_students_realloc_error;
			}
			ids = tmp_ids;
		}

		if (fscanf(in, "%u %s %s %hu %s", &(*students)[(*actual_size)].id, &(*students)[(*actual_size)].name, &(*students)[(*actual_size)].surname, &(*students)[(*actual_size)].course, &(*students)[(*actual_size)].group) != 5) {
			return read_students_invalid_data;
		}

		if (!((*students)[(*actual_size)].assessments = (short*)malloc(sizeof(short) * 5))) {
			clear(students, actual_size);
			free(ids);
			return read_students_malloc_error; 
		}

		for (i = 0; i < 5; i++) {
			if (fscanf(in, "%hu", &(*students)[(*actual_size)].assessments[i]) != 1) {
				*actual_size += 1;
				clear(students, actual_size);
				free(ids);
				return read_students_invalid_data;
			}
		}

		if (!is_valid((*students)[(*actual_size)], ids, (*actual_size))) {
			*actual_size += 1;
			clear(students, actual_size);
			free(ids);
			return read_students_invalid_data;
		}

		(*actual_size)++;
	}

	if (!(tmp = (student*)realloc((*students), sizeof(student) * (*actual_size)))) {
		clear(students, actual_size);
		free(ids);
		return read_students_realloc_error;
	}
	(*students) = tmp;

	free(ids);
	return read_students_ok;
}

find_statuses find(void* to_find, student* first, size_t number_arr, student** collection, size_t* number_coll, int comparator(static void*, static void*))
{
	if ((*collection) != NULL) {
		return find_incorrect_ptr_to_collection;
	}

	if (!first || number_arr <= 0) {
		return find_incorrect_ptr_to_first;
	}
	
	(*number_coll) = 0;
	size_t size = 4, i = 0;
	student* tmp = NULL;

	if (!((*collection) = (student*)malloc(sizeof(student) * size))) {
		return find_malloc_error;
	}

	for (i = 0; i < number_arr; i++) {
		if (!comparator(to_find, &(first[i]))) {
			if ((*number_coll) >= size - 1) {
				size *= 2;
				if (!(tmp = (student*)realloc((*collection), sizeof(student) * size))) {
					free(*collection);
					return find_realloc_error;
				}
				(*collection) = tmp;
			}

			(*collection)[(*number_coll)++] = first[i];
		}
	}

	if ((*number_coll) == 0) {
		return find_no_coincidence;
	}

	if (!(tmp = (student*)realloc((*collection), sizeof(student) * (*number_coll)))) {
		free(*collection);
		return find_realloc_error;
	}
	(*collection) = tmp;

	return find_ok;
}

int comp_id(static void* ptr1, static void* ptr2)
{
	return ((student*)ptr1)->id - ((student*)ptr2)->id;
}

int comp_name(static void* ptr1, static void* ptr2)
{
	return strcmp(((student*)ptr1)->name, ((student*)ptr2)->name);
}

int comp_surname(static void* ptr1, static void* ptr2)
{
	return strcmp(((student*)ptr1)->surname, ((student*)ptr2)->surname);
}

int comp_group(static void* ptr1, static void* ptr2)
{
	return strcmp(((student*)ptr1)->group, ((student*)ptr2)->group);
}

int comp_course(static void* ptr1, static void* ptr2)
{
	return ((student*)ptr1)->course - ((student*)ptr2)->course;
}

void print_students(FILE* stream, student* array, size_t number)
{
	size_t i, j;
	float average = 0;
	for (i = 0; i < number; i++) {
		fprintf(stream, "%s %s: course %hu; group %s\n", array[i].name, array[i].surname, array[i].course, array[i].group);
		for (j = 0; j < 5; j++) {
			average += array[i].assessments[j];
		}
		average /= 5;
		fprintf(stream, "Average assessment: %f\n\n", average);
		average = 0;
	}
}

// appends filePath with append before file resolution
char* generate_name(char* filePath, char* append)
{
	char* fptr = filePath, *lastDot = NULL, *name = NULL;
	int firstPartLength = 0, filePathLen = strlen(filePath), appendLen = strlen(append);
	while (*fptr) {
		if (*fptr == '.')
			lastDot = fptr;
		fptr++;
	}

	if (!(name = (char*)malloc(sizeof(char) * (filePathLen + appendLen + 1)))) {
		return NULL;
	}

	if (lastDot == NULL) { // filePath goes without resolution
		strcpy(name, filePath);
		strcpy(name + filePathLen, append);
	}
	else {
		strncpy(name, filePath, firstPartLength = lastDot - filePath);
		strcpy(name + firstPartLength, append);
		strcpy(name + firstPartLength + appendLen, lastDot);
	}
	return name;
}

int comp_above_average(static void* ptr1, static void* ptr2, float average)
{
	int i;
	float average_assess_student = 0;

	for (i = 0; i < 5; i++) {
		average_assess_student += ((student*)ptr2)->assessments[i];
	}
	average_assess_student /= 5;

	return (((student*)ptr1)->course == ((student*)ptr2)->course) && (average < average_assess_student);
}

find_statuses find_above_average(void* to_find, student* first, size_t number_arr, student** collection, size_t* number_coll, int comparator(static void*, static void*, float))
{
	if ((*collection) != NULL) {
		return find_incorrect_ptr_to_collection;
	}

	if (!first || number_arr <= 0) {
		return find_incorrect_ptr_to_first;
	}

	float average = 0, sub_average = 0;
	size_t i, j;
	for (i = 0; i < number_arr; i++) {
		for (j = 0; j < 5; j++) {
			sub_average += first[i].assessments[j];
		}
		average += sub_average / 5;
		sub_average = 0;
	}
	average /= number_arr;

	(*number_coll) = 0;
	size_t size = 4;
	student* tmp = NULL;

	if (!((*collection) = (student*)malloc(sizeof(student) * size))) {
		return find_malloc_error;
	}

	for (i = 0; i < number_arr; i++) {
		if (comparator(to_find, &(first[i]), average)) {
			if ((*number_coll) >= size - 1) {
				size *= 2;
				if (!(tmp = (student*)realloc((*collection), sizeof(student) * size))) {
					free(*collection);
					return find_realloc_error;
				}
				(*collection) = tmp;
			}

			(*collection)[(*number_coll)++] = first[i];
		}
	}

	if ((*number_coll) == 0) {
		return find_no_coincidence;
	}

	if (!(tmp = (student*)realloc((*collection), sizeof(student) * (*number_coll)))) {
		free(*collection);
		return find_realloc_error;
	}
	(*collection) = tmp;

	return find_ok;
}

above_average_statuses above_average(const char* filePath_in, student* students, int count)
{
	student to_find, * collection = NULL;
	unsigned int i;
	size_t number_coll = 0;
	char* fileNameOut = NULL, * append = NULL;
	FILE* fi_out = NULL;
	find_statuses find_s;

	for (i = 1; i < 5; i++) {
		to_find.course = i;
		find_s = find_above_average(&to_find, students, count, &collection, &number_coll, comp_above_average);
		switch (find_s) {
		case find_incorrect_ptr_to_collection:
			return above_average_incorrect_ptr_to_collection;

		case find_incorrect_ptr_to_first:
			return above_average_incorrect_ptr_to_first;

		case find_malloc_error:
			return above_average_malloc_error;

		case find_realloc_error:
			return above_average_realloc_error;
		case find_no_coincidence:
			free(collection);
			collection = NULL;
			continue;
		default:
			break;
		}

		append = (char*)malloc(sizeof(char) * 3);
		*append = '_';
		append[1] = i + 48;
		append[2] = '\0';
		if (!(fileNameOut = generate_name(filePath_in, append))) {
			free(collection);
			number_coll = 0;
			return above_average_malloc_error;
		}
		if (!(fi_out = fopen(fileNameOut, "w"))) {
			free(generate_name);
			free(collection);
			number_coll = 0;
			return above_average_open_file_error;
		}
		print_students(fi_out, collection, number_coll);
		free(fileNameOut);
		free(collection);
		collection = NULL;
		number_coll = 0;
		fclose(fi_out);
		free(append);
	}

	return above_average_ok;
}