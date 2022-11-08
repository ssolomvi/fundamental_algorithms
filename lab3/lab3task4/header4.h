#ifndef HEADER_4

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define fileNameLen 16

typedef struct message {
	unsigned int id;
	char* message;
	unsigned int len_bytes;
} message;

/// <summary>
/// Clears a dynamic array of struct message within index range [0; mess_count - 1]
/// </summary>
void clear(message** messes, int* mess_count);

void print_message_arr(FILE* stream, message* mess, int mess_count);

typedef enum read_message_statuses {
	read_message_incorrect_ptr_to_file,
	read_message_incorrect_ptr_to_stop_lexeme,
	read_message_realloc_error,
	read_message_fgets_error,
	read_message_stop_word_found,
	read_message_no_stop_word_found
} read_message_statuses;

/// <summary>
/// Reads a string until '\n' or EOF or stop_lexeme found and fulfills a message struct
/// </summary>
/// <param name="fi">- stream to read from</param>
/// <param name="mess">- returned value, read string for which memory is allocated in function read_message</param>
/// <param name="last_id">- last id, counts from 0</param>
/// <param name="stop_lexeme">- the lexeme which if found stops the reading</param>
/// <returns>read_message_statuses</returns>
read_message_statuses read_message(FILE* fi, message* mess, unsigned int* last_id, char* stop_lexeme);

typedef enum in_csv_file_statuses {
	in_csv_file_incorrect_file_ptr,
	in_csv_file_incorrect_message_passed,
	in_csv_file_malloc_error,
	in_csv_file_realloc_error,
	in_csv_file_ok
} in_csv_file_statuses;

/// <summary>
/// Puts a struct message to csv file
/// </summary>
/// <param name="fi">- file ptr</param>
/// <param name="mess">- variable of message type</param>
/// <returns>in_csv_file_statuses</returns>
in_csv_file_statuses in_csv_file(FILE* fi, message mess);

/// <summary>
/// Generates a fileName where digits and letters are equiprobable
/// </summary>
/// <returns>generated string</returns>
char* gen_file_name();

typedef enum read_from_csv_file_statuses {
	read_from_csv_file_incorrect_ptr_to_file,
	read_from_csv_file_incorrect_ptr_to_messages_arr,
	read_from_csv_file_malloc_error,
	read_from_csv_file_realloc_error,
	read_from_csv_file_ok
} read_from_csv_file_statuses;

/// <summary>
/// Reads messages from .csv file in dynamic array of struct message
/// </summary>
/// <param name="fi">- file to read from</param>
/// <param name="messes">- a dynamic array, must be NULL</param>
/// <param name="mess_count">- count of read messages</param>
/// <returns>read_from_csv_file_statuses</returns>
read_from_csv_file_statuses read_from_csv_file(FILE* fi, message** messes, int* mess_count);

typedef enum read_n_write_csv_statuses {
	read_n_write_malloc_error,
	read_n_write_realloc_error,
	read_n_write_file_ptr_error,
	read_n_write_incorrect_stop_lexeme,
	read_n_write_fgets_error,
	read_n_write_message_error,
	read_n_write_ok
} read_n_write_csv_statuses;

read_n_write_csv_statuses read_n_write_csv(FILE* _in, FILE* _out, char* stop_lexeme);

#endif // !HEADER_4