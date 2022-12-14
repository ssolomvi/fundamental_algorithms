#define _CRT_SECURE_NO_WARNINGS

#ifndef HEADER_10
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct signly_linked_list_node {
	char* string;
	struct signly_linked_list_node* prev;
} signly_linked_list_node;

typedef struct stack {
	signly_linked_list_node* top_element;
} stack;

typedef struct queue {
	unsigned int nodes_count;
	signly_linked_list_node* first_element, * last_element;
} queue;

typedef enum read_str_with_special_words_statuses {
	read_str_with_special_words_malloc_error,
	read_str_with_special_words_realloc_error,
	read_str_with_special_words_cancel,
	read_str_with_special_words_stop,
	read_str_with_special_words_ok
} read_str_with_special_words_statuses;

void delete_stack_n_queue(stack* st, queue* qu);
int insert_string(stack* st, queue* qu, char* str);
signly_linked_list_node* pop_node_from_queue(queue* qu, stack* st);
signly_linked_list_node* pop_node_from_stack(stack* st, queue* qu);
read_str_with_special_words_statuses read_str_with_special_words(FILE* stream, char** new_str);
int cancel(stack* st, queue* qu, unsigned int to_del);
void put_in_file(FILE* stream, stack* st, queue* qu, unsigned int to_put);

#endif // !HEADER_10
