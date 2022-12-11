#include "dbl_list.h"

typedef enum dbl_list_statuses {
	dbl_list_malloc_error,
	dbl_list_ok,
} dbl_list_statuses;

void delete_dbl_list(dbl_list* list)
{
	dbl_elem* ptr = list->head, * tmp = NULL;

	while (ptr != NULL) {
		delete_string(&(ptr->data->text));
		tmp = ptr;
		ptr = ptr->next;
		free(tmp);
	}
	list->head = NULL;
}

dbl_list_statuses init_dbl_list(dbl_list* list, T* data)
{
	if (!((*list).head = (dbl_elem*)malloc(sizeof(dbl_elem)))) {
		(*list).head = NULL;
		return dbl_list_malloc_error;
	}

	(*list).head->data = data;
	(*list).head->next = NULL;
	(*list).head->prev = NULL;
	return dbl_list_ok;
}

int compare_dbl_prior(T* first, T* second)
{
	return first->priority - second->priority;
}

dbl_list_statuses insert_dbl_list(dbl_list* list, T* data, int compare(T*, T*))
{
	if (!list->head) {
		init_dbl_list(list, data);
		if (!list->head)
			return dbl_list_malloc_error;
		return dbl_list_ok;
	}

	dbl_elem* ptr = list->head, *tmp = NULL, *new_elem = NULL;

	if (!(new_elem = (dbl_elem*)malloc(sizeof(dbl_elem)))) {
		delete_dbl_list(list);
		return dbl_list_malloc_error;
	}
	new_elem->data = data;

	while (ptr != NULL) {
		// insert before head, among 2 elements
		if (compare(data, ptr->data) > 0) {
			if (ptr->prev) {
				ptr->prev->next = new_elem;
			}

			new_elem->prev = ptr->prev;
			new_elem->next = ptr;
			ptr->prev = new_elem;

			if (ptr == list->head) {
				list->head = list->head->prev;
			}
			break;
		}

		// insert after the last element
		if (ptr->next == NULL) {
			new_elem->next = NULL;
			new_elem->prev = ptr;
			ptr->next = new_elem;
			break;
		}

		ptr = ptr->next;
	}

	return dbl_list_ok;
}

T* find_min_dbl_list(dbl_list list)
{
	if (!list.head || !(list.head->next))
		return NULL;

	dbl_elem* tmp = list.head;

	while (tmp->next) {
		tmp = tmp->next;
	}

	return tmp->data;
}

T* find_max_dbl_list(dbl_list list)
{
	if (!list.head)
		return NULL;

	return list.head->data;
}

dbl_elem* extract_min_dbl_list(dbl_list* list)
{
	if (!list->head)
		return NULL;

	dbl_elem* tmp = list->head, * to_return = NULL;

	if (!(list->head->next)) {
		to_return = list->head;
		list->head = NULL;
		return to_return;
	}

	while (tmp->next->next) {
		tmp = tmp->next;
	}

	to_return = tmp->next;
	tmp->next = NULL;
	return to_return;
}

dbl_elem* exctract_max_dbl_list(dbl_list* list)
{
	if (!list->head)
		return NULL;

	dbl_elem* to_return = list->head;

	if (!(list->head->next)) {
		list->head = NULL;
		return to_return;
	}

	list->head = list->head->next;
	list->head->prev = NULL;
	return to_return;
}

int make_new_T(FILE* fi, int priority, T** to_create)
{
	if (!((*to_create) = (T*)malloc(sizeof(T)))) {
		return -1;
	}

	(*to_create)->text = create_string(fi, 0, '\n');
	if ((*to_create)->text.length == 0) {
		delete_string(&(*to_create)->text);
		(*to_create) = NULL;
		return -2;  // file read
	}

	(*to_create)->priority = priority;
	return 0;
}

read_from_file_dbl_list_statuses read_from_file_dbl_list(dbl_list* list, container* input_arguments, size_t count_of_files)
{
	FILE* curr_fi = NULL;
	size_t count_of_read_files = 0, curr_reading_number = 0;
	int create_T_response = 0;
	T* new_T = NULL;
	dbl_list_statuses dbl_list_s = 1;

	while (count_of_read_files != count_of_files) {
		if (input_arguments[curr_reading_number].is_read)
			continue;

		if (!(curr_fi = fopen(input_arguments[curr_reading_number].data.text.str, "r"))) {
			delete_dbl_list(list);
			return read_from_file_dbl_list_error_opening_file;
		}

		if (fseek(curr_fi, input_arguments[curr_reading_number].bytes_read, SEEK_SET) != 0) {
			delete_dbl_list(list);
			return read_from_file_dbl_list_fseek_error;
		}

		if ((create_T_response = make_new_T(curr_fi, input_arguments[curr_reading_number].data.priority, &new_T)) == -1) {
			delete_dbl_list(list);
			return read_from_file_dbl_list_malloc_error;
		}

		if (create_T_response == -2) {
			input_arguments[curr_reading_number].is_read = 1;
			count_of_read_files++;
			curr_reading_number++;
			if (curr_reading_number == count_of_files) {
				curr_reading_number = 0;
			}
			continue;
		}

		if ((dbl_list_s = insert_dbl_list(list, new_T, compare_dbl_prior)) == dbl_list_malloc_error) {
			delete_string(&(new_T->text));
			delete_dbl_list(list);
			return read_from_file_dbl_list_malloc_error;
		}

		input_arguments[curr_reading_number].bytes_read += (new_T->text.length + 2) * sizeof(char);

		fclose(curr_fi);

		curr_reading_number++;
		if (curr_reading_number == count_of_files) {
			curr_reading_number = 0;
		}
	}


	return read_from_file_dbl_list_ok;
}

void print_T(FILE* stream, T* data)
{
	fprintf(stream, "Priority: %d\n%s\n\n", data->priority, data->text.str);
}

void extract_n_print(FILE* stream, dbl_list* list)
{
	if (!(list->head))
		return;

	dbl_elem* ptr = extract_min_dbl_list(list);

	while (ptr) {
		print_T(stream, ptr->data);
		delete_string(&(ptr->data->text));
		free(ptr->data);
		free(ptr);

		ptr = extract_min_dbl_list(list);
	}
}