#include "header10.h"

void delete_signly_linked_list_node(signly_linked_list_node* node)
{
	if (!node)
		return;
	free(node->string);
	free(node);
}

void delete_stack_n_queue(stack* st, queue* qu)
{
	signly_linked_list_node* ptr = st->top_element, * tmp = NULL;

	while (ptr) {
		tmp = ptr;
		ptr = ptr->prev;

		delete_signly_linked_list_node(tmp);
	}

	st->top_element = NULL;
	qu->first_element = NULL;
	qu->last_element = NULL;
	qu->nodes_count = 0;
}

int insert_string(stack* st, queue* qu, char* str)
{
	signly_linked_list_node* new_node = NULL;
	
	if (!(new_node = (signly_linked_list_node*)malloc(sizeof(signly_linked_list_node)))) {
		delete_stack_n_queue(st, qu);
		return -1;
	}
	if (!(new_node->string = (char*)malloc(sizeof(char) * (strlen(str) + 1)))) {
		delete_stack_n_queue(st, qu);
		free(new_node);
		return -1;
	}
	strcpy(new_node->string, str);
	
	if (!st->top_element) {
		new_node->prev = NULL;
		qu->first_element = new_node;
	}
	else {
		new_node->prev = st->top_element;
	}

	st->top_element = new_node;
	qu->last_element = new_node;
	qu->nodes_count++;

	return 0;
}

signly_linked_list_node* pop_node_from_queue(queue* qu, stack* st)
{
	if (!(qu->first_element)) {
		return NULL;
	}
	
	signly_linked_list_node* to_pop = qu->first_element, * tmp = qu->last_element;
	
	if (to_pop == tmp) {
		qu->first_element = NULL;
		qu->last_element = NULL;
		st->top_element = NULL;
		return to_pop;
	}
	qu->nodes_count--;

	while (tmp->prev != qu->first_element) {
		tmp = tmp->prev;
	}

	qu->first_element = tmp;
	tmp->prev = NULL;

	return to_pop;
}

signly_linked_list_node* pop_node_from_stack(stack* st, queue* qu)
{
	if (!(st->top_element))
		return NULL;

	signly_linked_list_node* to_pop = st->top_element;
	qu->nodes_count--;

	if (qu->first_element == qu->last_element) {
		qu->first_element = NULL;
		qu->last_element = NULL;
		st->top_element = NULL;
		return to_pop;
	}

	st->top_element = st->top_element->prev;
	qu->last_element = st->top_element;

	return to_pop;
}

read_str_with_special_words_statuses read_str_with_special_words(FILE* stream, char** new_str)
{
	if (!stream)
		stream = stdin;
	
	size_t allocated = 16, act_size = 0;
	char ch = 0, * tmp = NULL;

	if (!((*new_str) = (char*)malloc(sizeof(char) * allocated))) {
		(*new_str) = NULL;
		return read_str_with_special_words_malloc_error;
	}

	while ((ch = fgetc(stream)) != '\n' && ch != EOF) {
		if (act_size >= allocated) {
			allocated *= 2;
			if (!(tmp = (char*)realloc((*new_str), sizeof(char) * allocated))) {
				free(new_str);
				return read_str_with_special_words_realloc_error;
			}
			(*new_str) = tmp;
		}

		(*new_str)[act_size++] = ch;
	}

	if (!(tmp = (char*)realloc((*new_str), sizeof(char) * (act_size + 2)))) {
		free(new_str);
		return read_str_with_special_words_realloc_error;
	}
	(*new_str) = tmp;
	(*new_str)[act_size] = '\n';
	(*new_str)[act_size + 1] = '\0';

	if (!strcmp("cancel\n", (*new_str))) {
		free(*new_str);
		return read_str_with_special_words_cancel;
	}

	if (!strcmp("stop\n", (*new_str))) {
		free(*new_str);
		return read_str_with_special_words_stop;
	}

	return read_str_with_special_words_ok;
}

int cancel(stack* st, queue* qu, unsigned int to_del)
{
	if (qu->nodes_count < to_del) {
		return to_del - qu->nodes_count;
	}

	signly_linked_list_node* tmp = NULL;
	unsigned int i;
	
	for (i = 0; i < to_del; i++) {
		tmp = pop_node_from_stack(st, qu);
		delete_signly_linked_list_node(tmp);
	}

	return 0;
}

void put_in_file(FILE* stream, stack* st, queue* qu, unsigned int to_put)
{
	if (!stream || !(st->top_element))
		return;

	signly_linked_list_node* tmp = NULL;
	unsigned i;

	for (i = 0; i < to_put; i++) {
		tmp = pop_node_from_queue(qu, st);
		fprintf(stream, "%s", tmp->string);
		delete_signly_linked_list_node(tmp);
	}
}