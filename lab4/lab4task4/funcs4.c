#include "header4.h"

void delete_long_long_array(long_long_array* arr)
{
	free(arr->array);
	arr->array = NULL;
	arr->count = 0;
}

int is_digit(char ch)
{
	return ch >= '0' && ch <= '9';
}

funcs_statuses load(FILE* in, long_long_array* arr)
{
	if (!in)
		return funcs_incorrect_ptr_to_file_passed;
	if (arr->array)
		return funcs_incorrect_ptr_to_array_passed;

	char ch = 0, prev = 0;
	long long number = 0, *tmp = NULL;
	size_t allocated = 16;
	if (!(arr->array = (long long*)malloc(sizeof(long long) * allocated))) {
		return funcs_malloc_error;
	}
	arr->count = 0;

	while (1) {
		ch = fgetc(in);
		// number continuos or starts
		if (is_digit(ch)) {
			number *= 10;
			number += (long long)ch - '0';
		}
		// number ended
		else if (is_digit(prev)) {
			if (arr->count >= allocated) {
				allocated *= 2;
				if (!(tmp = (long long*)realloc(arr->array, sizeof(long long) * allocated))) {
					delete_long_long_array(arr);
					return funcs_realloc_error;
				}
				arr->array = tmp;
			}

			arr->array[arr->count++] = number;
			number = 0;
			
		}
		prev = ch;
		if (ch == EOF)
			break;
	}

	if (!(tmp = (long long*)realloc(arr->array, sizeof(long long) * arr->count))) {
		delete_long_long_array(arr);
		return funcs_realloc_error;
	}
	arr->array = tmp;

	return funcs_ok;
}

funcs_statuses save(FILE* out, long_long_array* arr)
{
	if (!out)
		return funcs_incorrect_ptr_to_file_passed;

	if (!(arr->array) || !(arr->count))
		return funcs_empty_arr;

	size_t i;
	for (i = 0; i < arr->count; i++) {
		fprintf(out, "%lld ", arr->array[i]);
		if (!(i % 10) && i != 0) {
			fputc('\n', out);
		}
	}

	return funcs_ok;
}

long long get_random(int min, int max)
{
	return ((long long)rand() % max + min);
}

funcs_statuses load_rand(long_long_array* arr, size_t count, int lb, int lr)
{
	if (arr->array)
		return funcs_incorrect_ptr_to_array_passed;

	if (!(arr->array = (long long*)malloc(sizeof(long long) * count))) {
		return funcs_malloc_error;
	}

	size_t i;
	for (i = 0; i < count; i++) {
		arr->array[i] = ((long long)rand() % (lr - lb) + lb);
	}

	arr->count = count;

	return funcs_ok;
}

funcs_statuses concat(long_long_array* A, long_long_array* B)
{
	if (!A->count && !B->count)
		return funcs_ok;

	if (A == B)
		return funcs_incorrect_ptr_to_array_passed;

	long long* tmp = NULL;
	if (!(tmp = (long long*)realloc(A->array, sizeof(long long) * (A->count + B->count)))) {
		delete_long_long_array(A);
		delete_long_long_array(B);
		return funcs_realloc_error;
	}
	A->array = tmp;

	size_t i;
	for (i = 0; i < B->count; i++) {
		A->array[i + A->count] = B->array[i];
	}

	A->count += B->count;

	return funcs_ok;
}

funcs_statuses remove_from_arr(long_long_array* arr, size_t start_index, size_t count_to_del)
{
	if (!(arr->count))
		return funcs_empty_arr;

	size_t sum = start_index + count_to_del, i;
	if (sum > arr->count)
		return funcs_error_index;

	if (sum != arr->count) {
		for (i = 0; i < arr->count - sum; i++) {
			arr->array[start_index + i] = arr->array[sum + i];
		}
	}
	
	arr->count -= count_to_del;

	long long* tmp = NULL;
	if (!(tmp = (long long*)realloc(arr->array, sizeof(long long) * arr->count))) {
		delete_long_long_array(arr);
		return funcs_realloc_error;
	}
	arr->array = tmp;

	return funcs_ok;
}

funcs_statuses copy(long_long_array* A, size_t start_ind, size_t end_ind, long_long_array* B)
{
	if (end_ind < start_ind || end_ind >= A->count)
		return funcs_error_index;

	if (B->array)
		return funcs_incorrect_ptr_to_array_passed;

	B->count = end_ind - start_ind + 1;

	if (!(B->array = (long long*)malloc(sizeof(long long) * B->count))) {
		delete_long_long_array(A);
		return funcs_malloc_error;
	}

	size_t i, j;
	for (j = 0, i = start_ind; i <= end_ind; j++, i++) {
		B->array[j] = A->array[i];
	}

	return funcs_ok;
}

int compare_non_decreasing(void* first, void* second)
{
	long long* fi = ((long long*)first), * se = ((long long*)second);
	
	return ((*fi) - (*se));
}

int compare_non_increasing(void* first, void* second)
{
	long long* fi = ((long long*)first), * se = ((long long*)second);

	return ((*se) - (*fi));
}

void sort(long_long_array* arr, int funct(void*, void*))
{
	if (!(arr->array) || arr->count == 0)
		return;
	qsort(arr->array, arr->count, sizeof(long long), funct);
}

void shuffle(long_long_array* arr)
{
	size_t current_index = arr->count, random_index = 0;
	long long tmp = 0;
	while (current_index != 0)
	{
		random_index = floor(rand() % arr->count);
		current_index--;
		tmp = arr->array[current_index];
		arr->array[current_index] = arr->array[random_index];
		arr->array[random_index] = tmp;
	}
}

void find_min_max_ind(long_long_array arr, size_t* max_i, size_t* min_i)
{
	if (!arr.array || arr.count == 0)
		return;

	(*max_i) = 0;
	(*min_i) = 0;

	size_t i;
	for (i = 0; i < arr.count; i++) {
		if (arr.array[(*max_i)] < arr.array[i])
			(*max_i) = i;
		if (arr.array[(*min_i)] > arr.array[i])
			(*min_i) = i;
	}
}

typedef struct times_met {
	long long number;
	size_t occurances;
} times_met;

int most_often_met(long_long_array arr, long long* elem)
{
	if (!arr.array || arr.count == 0)
		return;
	size_t allocated = 8, act_size = 1, i, j;
	times_met* help_arr = (times_met*)malloc(sizeof(times_met) * allocated), * tmp = NULL;
	if (!help_arr)
		return -1;

	help_arr[0].number = arr.array[0];
	help_arr[0].occurances = 1;

	int already_in_flag = 0;

	for (i = 1; i < arr.count; i++) {
		already_in_flag = 0;
		for (j = 0; j < act_size; j++) {
			if (arr.array[i] == help_arr[j].number) {
				help_arr[j].occurances++;
				already_in_flag = 1;
				break;
			}
		}
		if (!already_in_flag) {
			if (allocated <= act_size) {
				allocated *= 2;
				if (!(tmp = (times_met*)realloc(help_arr, sizeof(times_met) * allocated))) {
					free(help_arr);
					return -2;
				}
				help_arr = tmp;
			}

			help_arr[act_size].number = arr.array[i];
			help_arr[act_size++].occurances = 1;
		}
	}
	
	times_met max = help_arr[0];

	for (i = 1; i < act_size; i++) {
		if (help_arr[i].occurances > max.occurances) {
			max = help_arr[i];
		}
		else if (help_arr[i].occurances == max.occurances) {
			if (help_arr[i].number > max.number)
				max = help_arr[i];
		}
	}

	(*elem) = max.number;
	free(help_arr);
	return 0;
}

long long average(long_long_array arr)
{
	if (!arr.array || arr.count == 0)
		return;

	size_t i;
	long double mult = 1.0 / arr.count, aver_d = 0;
	for (i = 0; i < arr.count; i++) {
		aver_d += mult * arr.array[i];
	}
	return ((long long)floor(aver_d));
}

long long my_abs(long long n)
{
	return n >= 0 ? n : -n;
}

long long deviation(long_long_array arr, long long value)
{
	if (!arr.array || arr.count == 0)
		return;

	size_t i;
	long long max_deviation = 0, tmp = 0;
	for (i = 0; i < arr.count; i++) {
		if ((tmp = my_abs(value - arr.array[i])) > max_deviation)
			max_deviation = tmp;
	}
	return max_deviation;
}

int stats(FILE* stream, long_long_array* arr)
{
	if (!arr->array || arr->count == 0) {
		return 1;
	}

	size_t max_elem_ind = 0, min_elem_ind = 0;
	long long most_often_met_elem = 0, aver_lld = 0;

	fprintf(stream, "Size of array: %Iu\n", arr->count);

	find_min_max_ind(*arr, &max_elem_ind, &min_elem_ind);
	fprintf(stream, "Indexes and values of:\n");
	fprintf(stream, "\tmax element: %Iu: %lld\n", max_elem_ind, arr->array[max_elem_ind]);
	fprintf(stream, "\tmin element: %Iu: %lld\n", min_elem_ind, arr->array[min_elem_ind]);
	
	if (most_often_met(*arr, &most_often_met_elem) != 0) {
		return -1;
	}
	fprintf(stream, "Most often met element: %lld\n", most_often_met_elem);
	
	aver_lld = average(*arr);
	fprintf(stream, "Average value of elements: %lld\n", aver_lld);
	fprintf(stream, "Max deviation from average value: %lld\n", deviation(*arr, aver_lld));

	return 0;
}

funcs_statuses print_from_to(FILE* stream, long_long_array arr, size_t start_ind, size_t end_ind)
{
	if (arr.count == 0) {
		return funcs_empty_arr;
	}

	size_t i = 0;
	
	if (end_ind < start_ind || end_ind >= arr.count)
		return funcs_error_index;

	for (i = start_ind; i <= end_ind; i++) {
		fprintf(stream, "%lld ", arr.array[i]);
		if (i % 10 == 0 && i != 0)
			fprintf(stream, "\n");
	}
	fprintf(stream, "\n");
	return funcs_ok;
}