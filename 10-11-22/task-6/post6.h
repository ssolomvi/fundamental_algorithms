#ifndef POST_6
#pragma once

#include "string6.h"
#define index_count_of_numbers 6
#define mail_id_count_of_numbers 14
#define time_count_of_numbers 19 // {day}.{month}.{year} {hour}:{minutes}:{seconds}

typedef struct address {
	string city;
	string street;
	unsigned int house;
	unsigned int flat;
	char index[index_count_of_numbers];
} address;

typedef struct mail {
	address sender;
	address recipient;
	float weigth;
	char mail_id[mail_id_count_of_numbers];
	char time_of_creation[time_count_of_numbers];
	char delivery_time[time_count_of_numbers];
} mail;

typedef struct post {
	address* this_post_address;
	mail* this_post_mails;
	unsigned int count_of_mails;
} post;

typedef enum post_statuses {
	post_realloc_error,
	post_malloc_error,
	post_incorrect_ptr_to_collection_passed,
	post_incorrect_ptr_to_file_passed,
	post_incorrect_input_file_format,
	post_ok
} post_statuses;

address* get_address(FILE* stream);

post_statuses read_mails(FILE* stream, post* this_post);

/// <summary>
/// Reallocates a dynamic array of mails in struct post (+1), adds a new mail to the array
/// </summary>
/// <param name="this_post">- post where to add a new mail</param>
/// <param name="to_add">- mail to add</param>
/// <returns>post_statuses</returns>
post_statuses add_mail(post this_post, mail to_add);

/// <summary>
/// Finds a certain mail (first occurrence) using some comparator function
/// </summary>
/// <param name="to_find">- mail to find</param>
/// <param name="this_post">- post where to search for the mail</param>
/// <param name="compare">- function which compares to mails</param>
/// <param name="ind">- the index of found mail in this_post dynamic array of mails</param>
/// <returns>pointer to found mail</returns>
mail* find_mail(mail to_find, post this_post, int compare(mail, mail), size_t* ind);

/// <summary>
/// Deletes a mail from this_post dynamic array of mails. Finds a needed mail using mail_id_comparator
/// </summary>
/// <param name="to_delete">- mail to delete</param>
/// <param name="this_post">- post from where delete the mail</param>
void delete_mail(mail to_delete, post* this_post);

/// <summary>
/// Deletes the whole post, uses delete_string
/// </summary>
/// <param name="to_delete">- post to delete</param>
void delete_post(post* to_delete);

/// <summary>
/// Sortes this_post dynamic array of mails using qsort function
/// </summary>
/// <param name="this_post">- post to sort</param>
void sort_recipient_n_mail_id(post this_post);

/// <summary>
/// Finds a collection of mails which are delivered to current date. Uses time_comparator
/// </summary>
/// <param name="found">- found collection of mails. Essentially must be NULL</param>
/// <param name="count_of_found">- count of found elements in found collection</param>
/// <param name="this_post">- post where to search</param>
/// <param name="current_time"></param>
/// <returns>post_statuses</returns>
post_statuses find_delivered_mails(mail** found, unsigned int* count_of_found, post this_post, char current_time[time_count_of_numbers]);

/// <summary>
/// Finds a collection of mails which delivery date is expires accordingly to current date. Uses time_comparator
/// </summary>
/// <param name="found">- found collection of mails. Essentially must be NULL</param>
/// <param name="count_of_found">- count of found elements in found collection</param>
/// <param name="this_post">- post where to search</param>
/// <param name="current_time"></param>
/// <returns>post_statuses</returns>
post_statuses find_expired_mails(mail** found, unsigned int* count_of_found, post this_post, char current_time[time_count_of_numbers]);


/// <summary>
/// A lexicographic comparator of mails ids
/// </summary>
/// <param name="first"></param>
/// <param name="second"></param>
/// <returns>less than 0 if first is less than second, equal to 0 if first equals second, greater 0 if first is greater second</returns>
int mail_id_comparator(mail first, mail second);

/// <summary>
/// Primary compares via recipient index, secondarily via mail_id. Uses mail_id_comparator
/// </summary>
/// <param name="ptr1"></param>
/// <param name="ptr2"></param>
/// <returns>less than 0 if ptr1 is less than ptr2, equal to 0 if ptr1 equals ptr2, greater 0 if ptr1 is greater ptr2</returns>
int recipient_n_mail_id_comparator(const void* ptr1, const void* ptr2);

/// <summary>
/// Compares to dates (char*) of format {day}.{month}.{year} {hour}:{minutes}:{seconds}
/// </summary>
/// <param name="first"></param>
/// <param name="second"></param>
/// <returns>less than 0 if first is earlier than second, equal to 0 if first equals second, greater 0 if first is later second</returns>
int time_comparator(char first[time_count_of_numbers], char second[time_count_of_numbers]);

/// <summary>
/// Primary compares via time of creation, secondarily via mail_id. Uses mail_id comparator
/// </summary>
/// <param name="ptr1"></param>
/// <param name="ptr2"></param>
/// <returns>less than 0 if ptr1 is less (earlier) than ptr2, equal to 0 if ptr1 equals ptr2, greater 0 if ptr1 is greater (later) ptr2</returns>
int time_of_creation_n_mail_id_comparator(const void* ptr1, const void* ptr2);

int is_valid_time(char to_valid[time_count_of_numbers]);
int is_valid_mail_id(char mail_id[mail_id_count_of_numbers]);
int is_valid_mail(mail to_val);
int is_valid_address(address to_val);

void print_mail(FILE* stream, mail to_pr);
void print_post(FILE* stream, post to_pr);
void print_mail_collection(FILE* stream, mail* to_pr, size_t count);

#endif // !POST_6