<<<<<<< HEAD
﻿#include "header7.h"
#include <stdlib.h>
#include <stdarg.h>

void free_multi(void* ptr, ...)
{
	va_list v;

	void* to_free = ptr;

	va_start(v, ptr);

	while (to_free)
	{
		free(to_free);

		to_free = va_arg(v, void*);
	}

	va_end(v);
}

read_citizens_statuses read_string(FILE* in, char** str, char divider)
{
	size_t size = 16, actual_size = 0;
	char ch, * tmp = NULL;

	if (!((*str) = (char*)malloc(sizeof(char) * size))) {
		*str = NULL;
		return read_citizens_malloc_error;
	}

	while ((ch = fgetc(in)) != EOF && ch != divider) {
		if (actual_size >= size - 1) {
			size *= 2;
			if (!(tmp = (char*)realloc((*str), sizeof(char) * size))) {
				free(*str);
				*str = NULL;
				return read_citizens_realloc_error;
			}
			(*str) = tmp;
		}

		(*str)[actual_size++] = ch;
	}

	if (!(tmp = (char*)realloc((*str), sizeof(char) * (actual_size + 1)))) {
		free(*str);
		*str = NULL;
		return read_citizens_realloc_error;
	}
	(*str) = tmp;
	(*str)[actual_size] = '\0';
	return read_citizens_ok;
}

int read_sex(FILE* input)
{
	char* str = NULL;
	read_citizens_statuses status = read_string(input, &str, ' ');
	if (status != read_citizens_ok) 
		return 0;
	
	if (!strcmp(str, "male")) 
		return male;
	if (!strcmp(str, "female"))
		return female;
	return 0;
}

int is_valid_head_str(char* to_val)
{
	if (!isupper(*to_val))
		return 0;
	to_val++;

	while (*to_val != '\0') {
		if (!islower(*to_val))
			return 0;
		to_val++;
	}
	return 1;
}

int is_valid_citizen(citizen* to_val)
{
	if (!is_valid_head_str(to_val->surname) || !is_valid_head_str(to_val->name) || !is_valid_head_str(to_val->patronymic))
		return 0;

	if (to_val->birthday.year <= 1900 || to_val->birthday.month > 12 || to_val->birthday.month < 1 ||
		(to_val->sex == 0) || to_val->average_income <= 0)
		return 0;

	if (to_val->birthday.month == 2) {
		if ((to_val->birthday.year % 400 == 0) || (to_val->birthday.year % 4 == 0 && to_val->birthday.year % 100 != 0)) {
			if (!(to_val->birthday.day <= 29 && to_val->birthday.day > 0))
				return 0;
		}
		else if (!(to_val->birthday.day < 29 && to_val->birthday.day > 0))
			return 0;
	}

	int days_31[7] = { 1, 3, 5, 7, 8, 10, 12 }, days_30[4] = { 4, 6, 9, 11 }, i;

	for (i = 0; i < 7; i++) {
		if (to_val->birthday.month == days_31[i] && !(to_val->birthday.day <= 31 && to_val->birthday.day > 0))
			return 0;
	}

	for (i = 0; i < 4; i++) {
		if (to_val->birthday.month == days_30[i] && !(to_val->birthday.day <= 30 && to_val->birthday.day > 0))
			return 0;
	}

	return 1;
}

// при должном уровне фантазии, будем считать что тут тоже пойдёт под пиво (=
read_citizens_statuses read_citizen(FILE* input, citizen** data)
{
	if (!((*data) = (citizen*)malloc(sizeof(citizen)))) {
		// (*data) = NULL; - оно и так NULL же :)
		return read_citizens_malloc_error;
	}

	// никогда так не делай (наврал, простите)
	// (**data) = (citizen){ .average_income = 0 };

	// так наверное попроще:
	(*data)->average_income = 0; // не правда ли? (=

	read_citizens_statuses read_citizens_s = -1;

	// мальнький upd можно прикрутить - проверять на несколько сепараторов, а не на один (= ну да ладно...
	if ((read_citizens_s = read_string(input, &((*data)->surname), ' ') != read_citizens_ok) ||
		(read_citizens_s = read_string(input, &((*data)->name), ' ') != read_citizens_ok) ||
		(read_citizens_s = read_string(input, &((*data)->patronymic), '\n') != read_citizens_ok))
	{
		// тут очень опасно так чистить
		// если указатели не равны NULL перед вызовом read_string или не инициализируются внутри, можешь получить ошибки
		// free((*data)->surname);
		// free((*data)->name);
		// free((*data)->patronymic);

		free_multi((*data)->surname, (*data)->name, (*data)->patronymic, *data, NULL);

		return read_citizens_s;
	}

	if (fscanf(input, "%u.%u.%u ", &((*data)->birthday).day, &((*data)->birthday).month, &((*data)->birthday).year) != 3 ||
		!((*data)->sex = read_sex(input)) ||
		fscanf(input, "%f\n", &(*data)->average_income) != 1)
	{
		// free((*data)->surname);
		// free((*data)->name);
		// free((*data)->patronymic);

		free_multi((*data)->surname, (*data)->name, (*data)->patronymic, *data, NULL);

		return read_citizens_incorrect_data;
	}

	if (!is_valid_citizen(*data)) {
		// free((*data)->surname);
		// free((*data)->name);
		// free((*data)->patronymic);

		free_multi((*data)->surname, (*data)->name, (*data)->patronymic, *data, NULL);

		return read_citizens_incorrect_data;
	}
	return read_citizens_ok;
}

// тут всё норм
read_citizens_statuses copy_citizen(citizen* to_copy, citizen** copy)
{
	if (!to_copy)
		return read_citizens_incorrect_ptr_to_value;

	if (!to_copy->name || !to_copy->surname || !to_copy->patronymic)
		return read_citizens_incorrect_ptr_to_value;
	
	if (!((*copy) = (citizen*)malloc(sizeof(citizen))))
		return read_citizens_malloc_error;

	size_t surn_len = strlen(to_copy->surname), name_len = strlen(to_copy->name), patr_len = strlen(to_copy->patronymic);

	if (!((*copy)->surname = (char*)malloc(sizeof(char) * (surn_len + 1)))) {
		free(*copy);
		return read_citizens_malloc_error;
	}

	if (!((*copy)->name = (char*)malloc(sizeof(char) * (name_len + 1)))) {
		free(((*copy)->surname));
		free(*copy);
		return read_citizens_malloc_error;
	}

	if (!((*copy)->patronymic = (char*)malloc(sizeof(char) * (patr_len + 1)))) {
		free(((*copy)->name));
		free(((*copy)->surname));
		free(*copy);
		return read_citizens_malloc_error;
	}

	strcpy((*copy)->surname, to_copy->surname);
	strcpy((*copy)->name, to_copy->name);
	strcpy((*copy)->patronymic, to_copy->patronymic);

	(*copy)->birthday.year = to_copy->birthday.year; 
	(*copy)->birthday.month = to_copy->birthday.month; 
	(*copy)->birthday.day = to_copy->birthday.day;
	(*copy)->sex = to_copy->sex;
	(*copy)->average_income = to_copy->average_income;

	return read_citizens_ok;
}

/// <summary>
/// Inserts node after list element
/// </summary>
/// <param name="input">- input stream</param>
/// <param name="list">- node to insert after a new node</param>
/// <returns>read_citizens_statuses</returns>
read_citizens_statuses insert_node(citizen* to_copy, list_cit* list)
{
	list_cit* new = (list_cit*)malloc(sizeof(list_cit));
	if (!new) {
		return read_citizens_malloc_error;
	}

	read_citizens_statuses read_citizens_s = -1;

	*new = (list_cit){ .data = NULL, .next = NULL };

	read_citizens_s = copy_citizen(to_copy, &(new->data));

	if (read_citizens_s != read_citizens_ok) {
		free(new);
		return read_citizens_s;
	}

	list_cit* next = list->next;
	list->next = new;
	new->next = next;
	return read_citizens_ok;
}

void delete_citizen(citizen* del)
{
	free(del->surname);
	free(del->name);
	free(del->patronymic);
}

void delete_citizens(list_cit** to_del)
{
	list_cit* tmp = NULL;

	while (*to_del)
	{
		tmp = (*to_del);
		(*to_del) = (*to_del)->next;
		delete_citizen(tmp->data); // чистка данных внутри структуры, которая лежит в элементе списка
		free(tmp); // чистка самого элемента списка
	}
}

int date_converter(date d)
{
	return d.year * 10000 + d.month * 100 + d.day;
}

read_citizens_statuses read_citizens(FILE* input, list_cit** head)
{
	if (!input)
		return read_citizens_incorrect_ptr_to_file;

	if (!head)
		return read_citizens_incorrect_ptr_to_list;
	// бля, сложно придумать имя переменной уже))0)0
	list_cit* new_element;
	list_cit* curr = head, *temp;
	int new_element_date, current_element_date;
	read_citizens_statuses status = -1;
	citizen* new = NULL, *tmp = NULL;

	while (!feof(input))
	{
		if (!*head)
		{
			*head = (list_cit*)malloc(sizeof(list_cit));
			if (!*head)
			{
				// TODO: ну разберёшься тут чё делац
				// return не забудь
			}

			(*head)->next = NULL;
			status = read_citizen(input, &((*head)->data));
		}
		else
		{
			curr = *head;
			status = read_citizen(input, &new);
			if (status != read_citizens_ok) {
				delete_citizens(head);
				return status;
			}

			new_element_date = date_converter(new->birthday); // birthd to insert
			current_element_date = date_converter(curr->data->birthday); // birthd of current element

#pragma region new element alloc
			new_element = (list_cit*)malloc(sizeof(list_cit));
			if (!new_element)
			{
				// TODO: memory allocation error handling
			}

			new_element->data = NULL;

			status = copy_citizen(new, &new_element->data);
			// TODO: handle status
#pragma endregion

			// возможно, вставить нужно будет в начало списка
			if (new_element_date < current_element_date)
			{
				new_element->next = *head;
				*head = new_element;
			}
			else
			{
				// путешествуем по списку в поисках элемента, после которого нужно вставить новый считанный
				while (curr->next != NULL)
				{
					// нашли нужное место
					// если чувак самый олд, тогда остановимся на последнем элементе списка и после него добавим
					if (new_element_date < date_converter(curr->next->data->birthday))
					{
						break;
					}

					curr = curr->next;
				}

				temp = curr->next;
				curr->next = new_element;
				new_element->next = temp;
			}
			

			/*if (date1 > date2)
			{
				tmp = (*head)->data;
				curr = (*head)->next;
				status = copy_citizen(new, &((*head)->data));
				if (status != read_citizens_ok) {
					delete_citizens(head);
					return status;
				}
				// ля чё это такое ._.
				// head->next = &(list_cit) { .data = NULL };
				
				(*head)->next->data = tmp;
				(*head)->next->next = curr;
			}
			else {
				while (curr->next && curr->data && curr->next->data) {
					date3 = date_converter(curr->next->data->birthday);
					//date3 = curr->next->data->birthday.year * 10000 + curr->next->data->birthday.month * 100 + curr->next->data->birthday.day; // birthd of next to current 
					if (date1 <= date2 && date1 >= date3)
					{
						break;
					}
					
					curr = curr->next;
					date2 = date3; // ну по сути так и есть же... зачем дважды считать
					//date2 = curr->data->birthday.year * 10000 + curr->data->birthday.month * 100 + curr->data->birthday.day;
				}
				insert_node(new, curr);
			}*/

			delete_citizen(new);
		}

		// тут тоже нужно не просто выйти, а память подчистить ._.
		switch (status)
		{
		case read_citizens_malloc_error:
			return read_citizens_malloc_error;
		case read_citizens_incorrect_data:
			return read_citizens_incorrect_data;
		case read_citizens_incorrect_ptr_to_value:
			return read_citizens_incorrect_ptr_to_value;
		default:
			break;
		}
	}
	return read_citizens_ok;
}

void print_list(FILE* stream, list_cit* to_print)
{
	list_cit* tmp = to_print;
	while (tmp) {
		fprintf(stream, "%s %s %s\n", tmp->data->surname, tmp->data->name, tmp->data->patronymic);
		fprintf(stream, "%02u.%02u.%4u %s %.2f\n\n", tmp->data->birthday.day, tmp->data->birthday.month, tmp->data->birthday.year,
			(tmp->data->sex == male ? "male" : "female"), tmp->data->average_income);
		
		tmp = (tmp->next);
	}
=======
#include "header7.h"

read_citizens_statuses read_string(FILE* in, char** str, char divider)
{
	size_t size = 16, actual_size = 0;
	char ch, * tmp = NULL;

	if (!((*str) = (char*)malloc(sizeof(char) * size))) {
		*str = NULL;
		return read_citizens_malloc_error;
	}

	while ((ch = fgetc(in)) != EOF && ch != divider) {
		if (actual_size >= size - 1) {
			size *= 2;
			if (!(tmp = (char*)realloc((*str), sizeof(char) * size))) {
				free(*str);
				*str = NULL;
				return read_citizens_realloc_error;
			}
			(*str) = tmp;
		}

		(*str)[actual_size++] = ch;
	}

	if (!(tmp = (char*)realloc((*str), sizeof(char) * (actual_size + 1)))) {
		free(*str);
		*str = NULL;
		return read_citizens_realloc_error;
	}
	(*str) = tmp;
	(*str)[actual_size] = '\0';
	return read_citizens_ok;
}

int read_sex(FILE* input)
{
	char* str = NULL;
	read_citizens_statuses status = read_string(input, &str, ' ');
	if (status != read_citizens_ok) 
		return 0;
	
	if (!strcmp(str, "male")) 
		return male;
	if (!strcmp(str, "female"))
		return female;
	return 0;
}

int is_valid_head_str(char* to_val)
{
	if (!isupper(*to_val))
		return 0;
	to_val++;

	while (*to_val != '\0') {
		if (!islower(*to_val))
			return 0;
		to_val++;
	}
	return 1;
}

int is_valid_citizen(citizen to_val)
{
	if (!is_valid_head_str(to_val.surname) || !is_valid_head_str(to_val.name) || !is_valid_head_str(to_val.patronymic))
		return 0;

	if (to_val.birthday.year <= 1900 || to_val.birthday.month > 12 || to_val.birthday.month < 1 || 
		(to_val.sex == 0) || to_val.average_income <= 0)
		return 0;

	if (to_val.birthday.month == 2) {
		if ((to_val.birthday.year % 400 == 0) || (to_val.birthday.year % 4 == 0 && to_val.birthday.year % 100 != 0)) {
			if (!(to_val.birthday.day <= 29 && to_val.birthday.day > 0))
				return 0;
		}
		else if (!(to_val.birthday.day < 29 && to_val.birthday.day > 0))
			return 0;
	}

	int days_31[7] = { 1, 3, 5, 7, 8, 10, 12 }, days_30[4] = { 4, 6, 9, 11 }, i;

	for (i = 0; i < 7; i++) {
		if (to_val.birthday.month == days_31[i] && !(to_val.birthday.day <= 31 && to_val.birthday.day > 0))
			return 0;
	}

	for (i = 0; i < 4; i++) {
		if (to_val.birthday.month == days_30[i] && !(to_val.birthday.day <= 30 && to_val.birthday.day > 0))
			return 0;
	}

	return 1;
}

read_citizens_statuses read_citizen(FILE* input, citizen** data)
{
	if (!((*data) = (citizen*)malloc(sizeof(citizen)))) {
		(*data) = NULL;
		return read_citizens_malloc_error;
	}

	(**data) = (citizen){ .average_income = 0 };

	read_citizens_statuses read_citizens_s = -1;
	if ((read_citizens_s = read_string(input, &((*data)->surname), ' ') != read_citizens_ok) ||
		(read_citizens_s = read_string(input, &((*data)->name), ' ') != read_citizens_ok) ||
		(read_citizens_s = read_string(input, &((*data)->patronymic), '\n') != read_citizens_ok)) {
		if ((*data)->surname)
			free((*data)->surname);
		if ((*data)->name)
			free((*data)->name);
		if ((*data)->patronymic)
			free((*data)->patronymic);
		return read_citizens_s;
	}

	if (fscanf(input, "%u.%u.%u ", &(*data)->birthday.day, &(*data)->birthday.month, &(*data)->birthday.year) != 3 ||
		!((*data)->sex = read_sex(input)) || fscanf(input, "%f\n", &(*data)->average_income) != 1) {
		free((*data)->surname);
		free((*data)->name);
		free((*data)->patronymic);
		return read_citizens_incorrect_data;
	}

	if (!is_valid_citizen((**data))) {
		free((*data)->surname);
		free((*data)->name);
		free((*data)->patronymic);
		return read_citizens_incorrect_data;
	}
	return read_citizens_ok;
}

read_citizens_statuses copy_citizen(citizen* to_copy, citizen** copy)
{
	if (!((*copy) = (citizen*)malloc(sizeof(citizen))))
		return read_citizens_malloc_error;

	if (!to_copy)
		return read_citizens_incorrect_ptr_to_value;

	if (!to_copy->name || !to_copy->surname || !to_copy->patronymic)
		return read_citizens_incorrect_ptr_to_value;

	size_t surn_len = strlen(to_copy->surname), name_len = strlen(to_copy->name), patr_len = strlen(to_copy->patronymic);

	if (!((*copy)->surname = (char*)malloc(sizeof(char) * (surn_len + 1)))) {
		free(*copy);
		return read_citizens_malloc_error;
	}
	strcpy((*copy)->surname, to_copy->surname);

	if (!((*copy)->name = (char*)malloc(sizeof(char) * (name_len + 1)))) {
		free(((*copy)->surname));
		free(*copy);
		return read_citizens_malloc_error;
	}
	strcpy((*copy)->name, to_copy->name);

	if (!((*copy)->patronymic = (char*)malloc(sizeof(char) * (patr_len + 1)))) {
		free(((*copy)->name));
		free(((*copy)->surname));
		free(*copy);
		return read_citizens_malloc_error;
	}
	strcpy((*copy)->patronymic, to_copy->patronymic);

	(*copy)->birthday.year = to_copy->birthday.year; 
	(*copy)->birthday.month = to_copy->birthday.month; 
	(*copy)->birthday.day = to_copy->birthday.day;
	(*copy)->sex = to_copy->sex;
	(*copy)->average_income = to_copy->average_income;

	return read_citizens_ok;
}

/// <summary>
/// Inserts node after list element
/// </summary>
/// <param name="input">- input stream</param>
/// <param name="list">- node to insert after a new node</param>
/// <returns>read_citizens_statuses</returns>
read_citizens_statuses insert_node(citizen* to_copy, list_cit* list)
{
	list_cit* new = (list_cit*)malloc(sizeof(list_cit));
	if (!new) {
		return read_citizens_malloc_error;
	}

	read_citizens_statuses read_citizens_s = -1;

	*new = (list_cit){ .data = NULL, .next = NULL };

	read_citizens_s = copy_citizen(to_copy, &(new->data));

	if (read_citizens_s != read_citizens_ok) {
		free(new);
		return read_citizens_s;
	}

	list_cit* next = list->next;
	list->next = new;
	new->next = next;
	return read_citizens_ok;
}

void delete_citizen(citizen* del)
{
	if ((*del).surname)
		free((*del).surname);
	if ((*del).name)
		free((*del).name);
	if ((*del).patronymic)
		free((*del).patronymic);
	free(del);
}

void delete_citizens(list_cit** to_del)
{
	list_cit* tmp = NULL;
	while ((*to_del)) {
		tmp = (*to_del);
		(*to_del) = (*to_del)->next;
		delete_citizen(tmp->data);
		// free(tmp);
	}
}

read_citizens_statuses read_citizens(FILE* input, list_cit* head)
{
	if (!input)
		return read_citizens_incorrect_ptr_to_file;

	if (!head)
		return read_citizens_incorrect_ptr_to_list;

	list_cit* curr = head;
	int date1 = 0, date2 = 0, date3 = 0;
	read_citizens_statuses status = -1;
	citizen* new = NULL, *tmp = NULL;

	while (!feof(input)) {
		if (!(head->data)) {
			head->next = NULL;
			status = read_citizen(input, &(head->data));
		}
		else {
			curr = head;
			status = read_citizen(input, &new);
			if (status != read_citizens_ok) {
				delete_citizens(head);
				return status;
			}
			date1 = new->birthday.year * 10000 + new->birthday.month * 100 + new->birthday.day; // birthd to insert
			date2 = curr->data->birthday.year * 10000 + curr->data->birthday.month * 100 + curr->data->birthday.day; // birthd of current element

			if (date1 > date2) {
				tmp = head->data;
				curr = head->next;
				status = copy_citizen(new, &(head->data));
				if (status != read_citizens_ok) {
					delete_citizens(head);
					return status;
				}
				
				head->next = &(list_cit) { .data = NULL };
				head->next->data = tmp;
				head->next->next = curr;
			}
			else {
				while (curr->next && curr->data && curr->next->data) {
					date3 = curr->next->data->birthday.year * 10000 + curr->next->data->birthday.month * 100 + curr->next->data->birthday.day; // birthd of next to current 
					if (date1 <= date2 && date1 >= date3) {
						break;
					}
					curr = curr->next;
					date2 = curr->data->birthday.year * 10000 + curr->data->birthday.month * 100 + curr->data->birthday.day;
				}
				insert_node(new, curr);
			}

			delete_citizen(new);
		}
		switch (status)
		{
		case read_citizens_malloc_error:
			return read_citizens_malloc_error;
		case read_citizens_incorrect_data:
			return read_citizens_incorrect_data;
		case read_citizens_incorrect_ptr_to_value:
			return read_citizens_incorrect_ptr_to_value;
		default:
			break;
		}
	}
	return read_citizens_ok;
}

void print_list(FILE* stream, list_cit* to_print)
{
	list_cit* tmp = to_print;
	while (tmp) {
		fprintf(stream, "%s %s %s\n", tmp->data->surname, tmp->data->name, tmp->data->patronymic);
		fprintf(stream, "%02u.%02u.%4u %s %.2f\n\n", tmp->data->birthday.day, tmp->data->birthday.month, tmp->data->birthday.year,
			(tmp->data->sex == male ? "male" : "female"), tmp->data->average_income);
		
		tmp = (tmp->next);
	}
>>>>>>> origin/main
}