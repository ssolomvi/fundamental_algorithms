<<<<<<< HEAD
#ifndef HEADER_5

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const int suubj_count = 5;

typedef struct student {
	unsigned int id;
	char name[80];
	char surname[80];
	unsigned short course;
	char group[80];
	unsigned short* assessments;
} student;

typedef enum read_students_statuses {
	read_students_incorrect_ptr_to_array,
	read_students_malloc_error,
	read_students_realloc_error,
	read_students_invalid_data,
	read_students_ok
} read_students_statuses;

typedef enum find_statuses {
	find_malloc_error,
	find_realloc_error,
	find_incorrect_ptr_to_collection,
	find_incorrect_ptr_to_first,
	find_no_coincidence,
	find_ok
} find_statuses;

typedef enum above_average_statuses {
	above_average_open_file_error,
	above_average_malloc_error,
	above_average_realloc_error,
	above_average_incorrect_ptr_to_collection,
	above_average_incorrect_ptr_to_first,
	above_average_ok
} above_average_statuses;

void clear(student** array, int* allocated);

/// <summary>
/// Reads a dynamic array of students' data from file
/// </summary>
/// <param name="students">- array where the function puts the read data</param>
/// <param name="count">- count of read students' data</param>
/// <param name="in">- stream to read students' data from</param>
/// <returns>read_students_statuses</returns>
read_students_statuses read_students(student** students, int* count, FILE* in);

/// <summary>
/// Find a collection of students' data which match to_find properties
/// </summary>
/// <param name="to_find">- student* variable by which data to find is passed</param>
/// <param name="first">- an array where students' data is</param>
/// <param name="number_arr">- count of notes in array first</param>
/// <param name="collection">- found concidences</param>
/// <param name="number_coll">- count of notes in array collection</param>
/// <param name="comparator">- function by which could be comparable two students' data</param>
/// <returns>find_statuses</returns>
find_statuses find(void* to_find, student* first, size_t number_arr, student** collection, size_t* number_coll, int comparator(static void*, static void*));

int comp_id(static void* ptr1, static void* ptr2);
int comp_name(static void* ptr1, static void* ptr2);
int comp_surname(static void* ptr1, static void* ptr2);
int comp_group(static void* ptr1, static void* ptr2);
int comp_course(static void* ptr1, static void* ptr2);

/// <summary>
/// Prints array of student type in stream
/// </summary>
/// <param name="stream">- stream to put data into</param>
/// <param name="array">- array to print</param>
/// <param name="number">- number of elements in an array</param>
void print_students(FILE* stream, student* array, size_t number);

/// <summary>
/// Function to put in trace files students with assessments above average
/// </summary>
/// <param name="filePath_in">- example file name to form names for trace file of format {filePathIn}{course}</param>
/// <param name="students">- an array of students' data</param>
/// <param name="count">- number of elements in an array of students' data</param>
/// <returns>above_average_statuses</returns>
above_average_statuses above_average(const char* filePath_in, student* students, int count);

#endif // !HEADER_5
=======
#ifndef HEADER_5

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct student {
	unsigned int id;
	char name[80];
	char surname[80];
	unsigned short course;
	char group[80];
	unsigned short* assessments;
} student;

typedef enum read_students_statuses {
	read_students_incorrect_ptr_to_array,
	read_students_malloc_error,
	read_students_realloc_error,
	read_students_invalid_data,
	read_students_ok
} read_students_statuses;

typedef enum find_statuses {
	find_malloc_error,
	find_realloc_error,
	find_incorrect_ptr_to_collection,
	find_incorrect_ptr_to_first,
	find_no_coincidence,
	find_ok
} find_statuses;

typedef enum above_average_statuses {
	above_average_open_file_error,
	above_average_malloc_error,
	above_average_realloc_error,
	above_average_incorrect_ptr_to_collection,
	above_average_incorrect_ptr_to_first,
	above_average_ok
} above_average_statuses;

void clear(student** array, int* allocated);

/// <summary>
/// Reads a dynamic array of students' data from file
/// </summary>
/// <param name="students">- array where the function puts the read data</param>
/// <param name="count">- count of read students' data</param>
/// <param name="in">- stream to read students' data from</param>
/// <returns>read_students_statuses</returns>
read_students_statuses read_students(student** students, int* count, FILE* in);

/// <summary>
/// Find a collection of students' data which match to_find properties
/// </summary>
/// <param name="to_find">- student* variable by which data to find is passed</param>
/// <param name="first">- an array where students' data is</param>
/// <param name="number_arr">- count of notes in array first</param>
/// <param name="collection">- found concidences</param>
/// <param name="number_coll">- count of notes in array collection</param>
/// <param name="comparator">- function by which could be comparable two students' data</param>
/// <returns>find_statuses</returns>
find_statuses find(void* to_find, student* first, size_t number_arr, student** collection, size_t* number_coll, int comparator(static void*, static void*));

int comp_id(static void* ptr1, static void* ptr2);
int comp_name(static void* ptr1, static void* ptr2);
int comp_surname(static void* ptr1, static void* ptr2);
int comp_group(static void* ptr1, static void* ptr2);
int comp_course(static void* ptr1, static void* ptr2);

/// <summary>
/// Prints array of student type in stream
/// </summary>
/// <param name="stream">- stream to put data into</param>
/// <param name="array">- array to print</param>
/// <param name="number">- number of elements in an array</param>
void print_students(FILE* stream, student* array, size_t number);

/// <summary>
/// Function to put in trace files students with assessments above average
/// </summary>
/// <param name="filePath_in">- example file name to form names for trace file of format {filePathIn}{course}</param>
/// <param name="students">- an array of students' data</param>
/// <param name="count">- number of elements in an array of students' data</param>
/// <returns>above_average_statuses</returns>
above_average_statuses above_average(const char* filePath_in, student* students, int count);

#endif // !HEADER_5
>>>>>>> origin/main
