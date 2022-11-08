#include "header7.h"
#include <stdarg.h>
#include <ctype.h>

void free_multi(size_t num, ...)
{
	va_list v;
	va_start(v, num);
	
	void* to_free = NULL;

	while (num) {
		to_free = va_arg(v, void*);
		free(to_free);
		num--;
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
	while (isspace((ch = fgetc(in))));
	if (isspace(ch)) {
		(**str) = '\0';
		return read_citizens_incorrect_data;
	}
	else {
		(*str)[actual_size++] = ch;
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
		return read_citizens_malloc_error;
	}

	// никогда так не делай (наврал, простите)
	// (**data) = (citizen){ .average_income = 0 };

	read_citizens_statuses read_citizens_s = -1;

	// мальнький upd можно прикрутить - проверять на несколько сепараторов, а не на один (= ну да ладно...
	if ((read_citizens_s = read_string(input, &((*data)->surname), ' ') != read_citizens_ok) ||
		(read_citizens_s = read_string(input, &((*data)->name), ' ') != read_citizens_ok) ||
		(read_citizens_s = read_string(input, &((*data)->patronymic), '\n') != read_citizens_ok))
	{
		// тут очень опасно так чистить
		// если указатели не равны NULL перед вызовом read_string или не инициализируются внутри, можешь получить ошибки
		free_multi(4, (*data)->surname, (*data)->name, (*data)->patronymic, *data);
		return read_citizens_s;
	}

	if (fscanf(input, "%u.%u.%u ", &((*data)->birthday).day, &((*data)->birthday).month, &((*data)->birthday).year) != 3 ||
		!((*data)->sex = read_sex(input)) ||
		fscanf(input, "%f\n", &(*data)->average_income) != 1)
	{
		free_multi((*data)->surname, (*data)->name, (*data)->patronymic, *data, NULL);

		return read_citizens_incorrect_data;
	}

	if (!is_valid_citizen(*data)) {
		free_multi((*data)->surname, (*data)->name, (*data)->patronymic, *data, NULL);

		return read_citizens_incorrect_data;
	}
	return read_citizens_ok;
}

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
				// ну разберёшься тут чё делац
				// return не забудь
				return read_citizens_malloc_error;
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

			new_element_date = date_converter(new->birthday);
			current_element_date = date_converter(curr->data->birthday);

#pragma region new element alloc
			new_element = (list_cit*)malloc(sizeof(list_cit));
			if (!new_element)
			{
				// free new, destroy list
				delete_citizen(new);
				delete_citizens(head);
				return read_citizens_malloc_error;
			}

			new_element->data = NULL;

			status = copy_citizen(new, &new_element->data);
			if (status != read_citizens_ok) {
				free(new_element);
				delete_citizen(new);
				delete_citizens(head);
				return status;
			}
#pragma endregion

			// возможно, вставить нужно будет в начало списка
			if (new_element_date > current_element_date)
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
					if (new_element_date > date_converter(curr->next->data->birthday))
					{
						break;
					}

					curr = curr->next;
				}

				temp = curr->next;
				curr->next = new_element;
				new_element->next = temp;
			}

			delete_citizen(new);
		}

		// тут тоже нужно не просто выйти, а память подчистить ._.
		if (status != read_citizens_ok) {
			delete_citizens(head);
			return status;
		}
	}
	return read_citizens_ok;
}

void print_citizen(FILE* stream, citizen* to_print)
{
	fprintf(stream, "%s %s %s\n", to_print->surname, to_print->name, to_print->patronymic);
	fprintf(stream, "%02u.%02u.%4u %s %.2f\n\n", to_print->birthday.day, to_print->birthday.month, to_print->birthday.year,
		(to_print->sex == male ? "male" : "female"), to_print->average_income);
}

void print_list(FILE* stream, list_cit* to_print)
{
	if (!stream)
		return;
	if (!to_print || !to_print->data) {
		fprintf(stream, "Your list is empty!\n");
	}
	list_cit* tmp = to_print;
	while (tmp) {
		print_citizen(stream, tmp->data);
		tmp = (tmp->next);
	}
}

int lexic_compare_snp(const void* ptr1, const void* ptr2)
{
	int diff = 0;
	citizen* first = ((citizen*)ptr1), * second = ((citizen*)ptr2);
	if ((diff = strcmp(((citizen*)ptr1)->surname, ((citizen*)ptr2)->surname))) {
		return diff;
	}
	
	if ((diff = strcmp(((citizen*)ptr1)->name, ((citizen*)ptr2)->name))) {
		return diff;
	}
	
	if ((diff = strcmp(((citizen*)ptr1)->patronymic, ((citizen*)ptr2)->patronymic))) {
		return diff;
	}
	return 0;
}

int delete_n_find_citizen(list_cit** head, citizen* to_delete, int comparator(const void*, const void*))
{
	list_cit* ptr = (*head), * tmp_next = NULL;

	if (!comparator((*head)->data, to_delete)) {
		(*head) = (*head)->next;
		delete_citizen(ptr->data);
		free(ptr);
		return 1;
	}

	// redo if it's the last element in the list
	while (ptr->next != NULL) {
		if (!comparator(ptr->next->data, to_delete)) {
			tmp_next = ptr->next->next;
			delete_citizen(ptr->next->data);
			free(ptr->next);
			ptr->next = tmp_next;
			return 1;
		}
		ptr = ptr->next;
	}

	return 0;
}

citizen* search(list_cit* head, citizen* to_find, int comparator(const void*, const void*))
{
	if (!head || !to_find)
		return NULL;
	list_cit* tmp = head;

	while (tmp) {
		if (!comparator(to_find, tmp->data)) {
			return tmp->data;
		}
		tmp = tmp->next;
	}
	return NULL;
}