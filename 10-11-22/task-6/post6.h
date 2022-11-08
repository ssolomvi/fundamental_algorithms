#ifndef POST_6
#pragma once

#include "string6.h"
#define index_count_of_numbers 6
#define mail_id_count_of_numbers 14
// {day}.{month}.{year} {hour}:{minutes}:{seconds}

typedef struct address {
	string city;
	string street;
	unsigned int house;
	unsigned int flat;
	char index[index_count_of_numbers];
} address;

typedef struct time {
	unsigned year;
	unsigned month;
	unsigned day;
	unsigned hour;
	unsigned min;
	unsigned sec;
} time;

typedef struct mail {
	address sender;
	address recipient;
	float weigth;
	char mail_id[mail_id_count_of_numbers];
	time time_of_creation;
	time delivery_time;
} mail;

typedef struct post {
	address* this_post_address;
	mail* this_post_mails;
	size_t count_of_mails;
} post;

typedef enum post_statuses {
	post_realloc_error,
	post_malloc_error,
	post_incorrect_ptr_to_collection_passed,
	post_incorrect_ptr_to_file_passed,
	post_incorrect_input_file_format,
	post_empty_post,
	post_no_match,
	post_ok
} post_statuses;

void delete_address(address* to_del);
void delete_mail_array(mail** arr, size_t* count_of_mails);

/// <summary>
/// Deletes the whole post, uses delete_string
/// </summary>
/// <param name="to_delete">- post to delete</param>
void delete_post(post* to_delete);


int is_valid_mail_id(char mail_id[mail_id_count_of_numbers]);
int is_valid_time(time to_val);
int is_valid_address(address to_val);
int is_valid_mail(mail to_val);

void get_address(FILE* stream, address* to_get);
int get_time(FILE* stream, mail* to_get);
int get_mail(FILE* stream, mail* to_get);

post_statuses read_mails(FILE* file_in, post* this_post);

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
post_statuses delete_mail(mail to_delete, post* this_post);


/// <summary>
/// Sortes this_post dynamic array of mails using qsort function. Primary compares via recipient index, secondarily via mail_id. Uses mail_id_comparator
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
post_statuses find_delivered_mails(mail** found, size_t* count_of_found, post this_post, time current_time);

/// <summary>
/// Finds a collection of mails which delivery date is expires accordingly to current date. Uses time_comparator
/// </summary>
/// <param name="found">- found collection of mails. Essentially must be NULL</param>
/// <param name="count_of_found">- count of found elements in found collection</param>
/// <param name="this_post">- post where to search</param>
/// <param name="current_time"></param>
/// <returns>post_statuses</returns>
post_statuses find_expired_mails(mail** found, size_t* count_of_found, post this_post, time current_time);

void print_mail(FILE* stream, mail to_pr);
void print_mail_collection(FILE* stream, mail* to_pr, size_t count);
void print_post(FILE* stream, post to_pr);


/// <summary>
/// A lexicographic comparator of mails ids
/// </summary>
/// <param name="first"></param>
/// <param name="second"></param>
/// <returns>less than 0 if first is less than second, equal to 0 if first equals second, greater 0 if first is greater second</returns>
int mail_id_comparator(mail first, mail second);

#endif // !POST_6