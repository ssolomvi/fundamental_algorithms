<<<<<<< HEAD
#include "header3.h"

void print_employees(FILE* stream, employee* array, int size)
{
	int i;
	for (i = 0; i < size; i++) {
		fprintf(stream, "%u %s %s %lf\n", array[i].id, array[i].name, array[i].surname, array[i].wage);
	}
}

void print_employee(FILE* stream, employee to_print)
{
	fprintf(stream, "%u %s %s %lf\n", to_print.id, to_print.name, to_print.surname, to_print.wage);
}

int is_valid_str(char* str)
{
	int str_len = strlen(str), i;
	for (i = 0; i < str_len; i++) {
		if (!isalpha(str[i])) {
			return 0;
		}
	}
	return 1;
}

int is_valid(unsigned int* ids, int ids_size, char* name, char* surname, double salary)
{
	int i;
	unsigned int to_check = ids[ids_size];

	for (i = 0; i < ids_size; i++) {
		if (ids[i] == to_check) {
			return 0;
		}
	}

	if (!is_valid_str(name) || !is_valid_str(surname)) {
		return 0;
	}

	if (salary < 0) {
		return 0;
	}

	return 1;
}

employee* read_employees(FILE* fi, int* actual_size, read_employees_statuses* read_employee_s)
{
	if (!fi) {
		(*read_employee_s) = read_employees_file_error;
		return NULL;
	}

	int size = 16;
	(*actual_size) = 0;
	employee* employee_array = (employee*)malloc(sizeof(employee) * size), * tmp_e = NULL;
	if (!employee_array) {
		(*read_employee_s) = read_employees_malloc_error;
		return NULL;
	}

	unsigned int id = 0, * ids = (unsigned int*)malloc(sizeof(unsigned int) * size), tmp_i;
	if (!ids) {
		free(employee_array);
		(*read_employee_s) = read_employees_malloc_error;
		return NULL;
	}

	double salary = 0;
	char ch = 0;


	while (!feof(fi)) {
		if ((*actual_size) >= size - 1) {
			size *= 2;
			if (!(tmp_e = (employee*)realloc(employee_array, sizeof(employee) * size)) || !(tmp_i = (unsigned int*)realloc(ids, sizeof(unsigned int) * size))) {
				free(employee_array);
				free(ids);
				(*read_employee_s) = read_employees_realloc_error;
				return NULL;
			}
			employee_array = tmp_e;
			ids = tmp_i;
		}
		

		if (fscanf(fi, "%u %s %s %lf\n", &id, employee_array[(*actual_size)].name, employee_array[(*actual_size)].surname, &salary) != 4) {
			while (ch != '\n') 
				ch = fgetc(fi);
			continue;
		}

		ids[(*actual_size)] = id;

		if (!is_valid(ids, (*actual_size), employee_array[(*actual_size)].name, employee_array[(*actual_size)].surname, salary)) {
			continue;
		}


		employee_array[(*actual_size)].id = id;
		employee_array[(*actual_size)++].wage = salary;
	}

	if (!(tmp_e = (employee*)realloc(employee_array, sizeof(employee) * (*actual_size)))) {
		free(employee_array);
		free(ids);
		(*read_employee_s) = read_employees_realloc_error;
		return NULL;
	}

	free(ids);
	(*read_employee_s) = read_employees_ok;
	return employee_array;
}

void initialize_heap(fibonacci_heap* heap)
{
	heap->root = NULL;
	heap->size = 0;
}

fibonacci_insert_statuses fibonacci_insert_node(char operation, fibonacci_heap* heap, employee* to_insert)
{
	if (!heap) {
		return fibonacci_insert_heap_incorrect_heap_ptr;
	}

	heapNode* new_node = (heapNode*)malloc(sizeof(heapNode));
	if (!new_node) {
		return fibonacci_insert_malloc_error;
	}

	new_node->key = to_insert;
	new_node->child = NULL;
	new_node->degree = 0;
	new_node->parent = NULL;

	if (heap->size == 0) {
		heap->root = new_node;
		heap->root->right = new_node;
		heap->root->left = new_node;
	}

	else {
		heapNode* prev_right = heap->root->right;
		heap->root->right = new_node;
		new_node->left = heap->root;
		new_node->right = prev_right;
		prev_right->left = new_node;
	}

	if (operation == '<' && (new_node->key->wage < heap->root->key->wage) || operation == '>' && (new_node->key->wage > heap->root->key->wage)) {
		heap->root = new_node;
	}

	heap->size++;
	return fibonacci_insert_ok;
}

void fibonacci_union_trees(heapNode* first, heapNode* second)
{
	heapNode* left = first->left, * right = second->right;
	second->right = first;
	first->left = second;
	left->right = right;
	right->left = left;
}

// previously child_node was a root node, thus it has no siblings but root ones
void fibonacci_link(fibonacci_heap* heap, heapNode* parent_node, heapNode* child_node)
{
	// delete child from its previous circular list
	child_node->left->right = child_node->right;
	child_node->right->left = child_node->left;
	child_node->left = NULL; child_node->right = NULL;
	
	// make child_node a son node of parent_node
	(parent_node->degree)++;
	if (parent_node->child != NULL) {
		child_node->parent = parent_node;
		child_node->right = parent_node->child->right;
		child_node->left = parent_node->child;
		parent_node->child->right->left = child_node;
		parent_node->child->right = child_node;
	}
	else {
		parent_node->child = child_node;
		child_node->parent = parent_node;
		child_node->left = child_node;
		child_node->right = child_node;
	}
	
}

typedef enum fibonacci_extract_n_consolidate_statuses {
	fibonacci_extract_n_consolidate_malloc_error,
	fibonacci_extract_n_consolidate_ok
} fibonacci_extract_n_consolidate_statuses;

fibonacci_extract_n_consolidate_statuses fibonacci_consolidate(char operation, fibonacci_heap* heap)
{
	int array_size = (int)floor(log2(heap->size)) + 1;
	// array to check if degrees do not repeat
	heapNode** array_of_trees_degrees = (heapNode**)calloc(array_size, sizeof(heapNode*));
	if (!array_of_trees_degrees)
		return fibonacci_extract_n_consolidate_malloc_error;

	// initialize an array with NULL ptrs
	int i;
	for (i = 0; i < array_size; i++) {
		array_of_trees_degrees[i] = NULL;
	}

	// do for every node in root circular list
	heapNode* curr_node = heap->root, *another_node = NULL;
	int curr_degree = curr_node->degree;

	do {
		// check if any other tree has the same degree
		while (array_of_trees_degrees[curr_degree] != NULL) {
			// found another tree with the same degree as current
			another_node = array_of_trees_degrees[curr_degree];

			if (another_node == curr_node) {
				break;
			}

			if ((operation == '<' && (curr_node->key->wage > another_node->key->wage)) || operation == '>' && (curr_node->key->wage < another_node->key->wage)) {
				SWAP(heapNode*, curr_node, another_node);
			}

			// make an another node a child of curr_node
			fibonacci_link(heap, curr_node, another_node);
			array_of_trees_degrees[curr_degree] = NULL;
			curr_degree++;
		}

		if (((operation == '<' && (curr_node->key->wage < heap->root->key->wage)) || operation == '>' && (curr_node->key->wage > heap->root->key->wage)) && curr_node != heap->root) {
			heap->root = curr_node;
		}
		if (another_node != curr_node)
			array_of_trees_degrees[curr_degree] = curr_node;

		curr_node = curr_node->right;
		curr_degree = curr_node->degree;
	} while (curr_node != heap->root);

	heap->root = NULL;
	
	// make new links between members of an array
	for (i = 0; i < array_size; i++) {
		curr_node = array_of_trees_degrees[i];
		if (curr_node != NULL) {
			// start building new links
			if (heap->root == NULL) {
				heap->root = curr_node;
				heap->root->left = curr_node;
				heap->root->right = curr_node;
			}

			// new connections already appeared
			else {
				// insert a node in root array
				curr_node->left = heap->root;
				curr_node->right = heap->root->right;
				heap->root->right->left = curr_node;
				heap->root->right = curr_node;

				// refresh root if needed
				if ((operation == '<' && (curr_node->key->wage < heap->root->key->wage)) || operation == '>' && (curr_node->key->wage > heap->root->key->wage)) {
					heap->root = curr_node;
				}
			}
		}
	}

	free(array_of_trees_degrees);
	return fibonacci_extract_n_consolidate_ok;
}

fibonacci_extract_n_consolidate_statuses fibonacci_extract_root_node(char operation, fibonacci_heap* heap, heapNode** extracted)
{
	heapNode* root = heap->root;
	if (root != NULL) {
		// adds every son element of root to circular list of root
		if (root->child != NULL) {
			heapNode* curr_node = root->child, * next_node = NULL;
			do {
				next_node = curr_node->right;
				curr_node->parent = NULL;

				curr_node->left = root;
				curr_node->right = root->right;
				root->right->left = curr_node;
				root->right = curr_node;

				curr_node = next_node;
			} while (curr_node != root->child);
		}

		// delete root element from root list; now it's previous root element
		root->left->right = root->right;
		root->right->left = root->left;
		
		// if pyramid consists of 1 element
		if (root->right == root) {
			heap->root = NULL;
		}
		else {
			fibonacci_extract_n_consolidate_statuses fib_con_s = -1;

			heap->root = root->right;
			fib_con_s = fibonacci_consolidate(operation, heap);
			if (fib_con_s == fibonacci_extract_n_consolidate_malloc_error) {
				return fibonacci_extract_n_consolidate_malloc_error;
			}
		}

		heap->size--;
	}
	(*extracted) = root;
	return fibonacci_extract_n_consolidate_ok;
}

fibonacci_heap_sort_statuses fibonacci_heap_sort(char operation, employee* array, int arr_size, FILE* fi)
{
	fibonacci_heap heap;
	initialize_heap(&heap);

	int i;
	fibonacci_insert_statuses fib_ins_s = -1;
	for (i = 0; i < arr_size; i++) {
		fib_ins_s = fibonacci_insert_node(operation, &heap, array + i);
		switch (fib_ins_s)
		{
		case fibonacci_insert_heap_incorrect_heap_ptr:
			return fibonacci_heap_sort_unexpected_error;
		case fibonacci_insert_malloc_error:
			return fibonacci_heap_sort_malloc_error;
		default:
			break;
		}
	}

	heapNode* extracted = NULL;
	fibonacci_extract_n_consolidate_statuses fib_extr_n_con_s = -1;
	for (i = 0; i < arr_size; i++) {
		fib_extr_n_con_s = fibonacci_extract_root_node(operation, &heap, &extracted);

		if (fib_extr_n_con_s == fibonacci_extract_n_consolidate_malloc_error) {
			return fibonacci_heap_sort_malloc_error;
		}

		print_employee(fi, *(extracted->key));
		free(extracted);
		extracted = NULL;
	}

	return fibonacci_heap_sort_ok;
}
=======
#include "header3.h"

void print_employees(FILE* stream, employee* array, int size)
{
	int i;
	for (i = 0; i < size; i++) {
		fprintf(stream, "%u %s %s %lf\n", array[i].id, array[i].name, array[i].surname, array[i].wage);
	}
}

void print_employee(FILE* stream, employee to_print)
{
	fprintf(stream, "%u %s %s %lf\n", to_print.id, to_print.name, to_print.surname, to_print.wage);
}

int is_valid_str(char* str)
{
	int str_len = strlen(str), i;
	for (i = 0; i < str_len; i++) {
		if (!isalpha(str[i])) {
			return 0;
		}
	}
	return 1;
}

int is_valid(unsigned int* ids, int ids_size, char* name, char* surname, double salary)
{
	int i;
	unsigned int to_check = ids[ids_size];

	for (i = 0; i < ids_size; i++) {
		if (ids[i] == to_check) {
			return 0;
		}
	}

	if (!is_valid_str(name) || !is_valid_str(surname)) {
		return 0;
	}

	if (salary < 0) {
		return 0;
	}

	return 1;
}

employee* read_employees(FILE* fi, int* actual_size, read_employees_statuses* read_employee_s)
{
	if (!fi) {
		(*read_employee_s) = read_employees_file_error;
		return NULL;
	}

	int size = 16;
	(*actual_size) = 0;
	employee* employee_array = (employee*)malloc(sizeof(employee) * size), * tmp_e = NULL;
	if (!employee_array) {
		(*read_employee_s) = read_employees_malloc_error;
		return NULL;
	}

	unsigned int id = 0, * ids = (unsigned int*)malloc(sizeof(unsigned int) * size), tmp_i;
	if (!ids) {
		free(employee_array);
		(*read_employee_s) = read_employees_malloc_error;
		return NULL;
	}

	double salary = 0;
	char ch = 0;


	while (!feof(fi)) {
		if ((*actual_size) >= size - 1) {
			size *= 2;
			if (!(tmp_e = (employee*)realloc(employee_array, sizeof(employee) * size)) || !(tmp_i = (unsigned int*)realloc(ids, sizeof(unsigned int) * size))) {
				free(employee_array);
				free(ids);
				(*read_employee_s) = read_employees_realloc_error;
				return NULL;
			}
			employee_array = tmp_e;
			ids = tmp_i;
		}
		

		if (fscanf(fi, "%u %s %s %lf\n", &id, employee_array[(*actual_size)].name, employee_array[(*actual_size)].surname, &salary) != 4) {
			while (ch != '\n') 
				ch = fgetc(fi);
			continue;
		}

		ids[(*actual_size)] = id;

		if (!is_valid(ids, (*actual_size), employee_array[(*actual_size)].name, employee_array[(*actual_size)].surname, salary)) {
			continue;
		}


		employee_array[(*actual_size)].id = id;
		employee_array[(*actual_size)++].wage = salary;
	}

	if (!(tmp_e = (employee*)realloc(employee_array, sizeof(employee) * (*actual_size)))) {
		free(employee_array);
		free(ids);
		(*read_employee_s) = read_employees_realloc_error;
		return NULL;
	}

	free(ids);
	(*read_employee_s) = read_employees_ok;
	return employee_array;
}

void initialize_heap(fibonacci_heap* heap)
{
	heap->root = NULL;
	heap->size = 0;
}

fibonacci_insert_statuses fibonacci_insert_node(char operation, fibonacci_heap* heap, employee* to_insert)
{
	if (!heap) {
		return fibonacci_insert_heap_incorrect_heap_ptr;
	}

	heapNode* new_node = (heapNode*)malloc(sizeof(heapNode));
	if (!new_node) {
		return fibonacci_insert_malloc_error;
	}

	new_node->key = to_insert;
	new_node->child = NULL;
	new_node->degree = 0;
	new_node->parent = NULL;

	if (heap->size == 0) {
		heap->root = new_node;
		heap->root->right = new_node;
		heap->root->left = new_node;
	}

	else {
		heapNode* prev_right = heap->root->right;
		heap->root->right = new_node;
		new_node->left = heap->root;
		new_node->right = prev_right;
		prev_right->left = new_node;
	}

	if (operation == '<' && (new_node->key->wage < heap->root->key->wage) || operation == '>' && (new_node->key->wage > heap->root->key->wage)) {
		heap->root = new_node;
	}

	heap->size++;
	return fibonacci_insert_ok;
}

void fibonacci_union_trees(heapNode* first, heapNode* second)
{
	heapNode* left = first->left, * right = second->right;
	second->right = first;
	first->left = second;
	left->right = right;
	right->left = left;
}

// previously child_node was a root node, thus it has no siblings but root ones
void fibonacci_link(fibonacci_heap* heap, heapNode* parent_node, heapNode* child_node)
{
	// delete child from its previous circular list
	child_node->left->right = child_node->right;
	child_node->right->left = child_node->left;
	child_node->left = NULL; child_node->right = NULL;
	
	// make child_node a son node of parent_node
	(parent_node->degree)++;
	if (parent_node->child != NULL) {
		child_node->parent = parent_node;
		child_node->right = parent_node->child->right;
		child_node->left = parent_node->child;
		parent_node->child->right->left = child_node;
		parent_node->child->right = child_node;
	}
	else {
		parent_node->child = child_node;
		child_node->parent = parent_node;
		child_node->left = child_node;
		child_node->right = child_node;
	}
	
}

typedef enum fibonacci_extract_n_consolidate_statuses {
	fibonacci_extract_n_consolidate_malloc_error,
	fibonacci_extract_n_consolidate_ok
} fibonacci_extract_n_consolidate_statuses;

fibonacci_extract_n_consolidate_statuses fibonacci_consolidate(char operation, fibonacci_heap* heap)
{
	int array_size = log2(heap->size) + 1;
	// array to check if degrees do not repeat
	heapNode** array_of_trees_degrees = (heapNode**)calloc(array_size, sizeof(heapNode*));
	if (!array_of_trees_degrees)
		return fibonacci_extract_n_consolidate_malloc_error;

	// initialize an array with NULL ptrs
	int i;
	for (i = 0; i < array_size; i++) {
		array_of_trees_degrees[i] = NULL;
	}

	// do for every node in root circular list
	heapNode* curr_node = heap->root, *another_node = NULL;
	int curr_degree = curr_node->degree;

	do {
		// check if any other tree has the same degree
		while (array_of_trees_degrees[curr_degree] != NULL) {
			// found another tree with the same degree as current
			another_node = array_of_trees_degrees[curr_degree];

			if (another_node == curr_node) {
				break;
			}

			if ((operation == '<' && (curr_node->key->wage > another_node->key->wage)) || operation == '>' && (curr_node->key->wage < another_node->key->wage)) {
				SWAP(heapNode*, curr_node, another_node);
			}

			// make an another node a child of curr_node
			fibonacci_link(heap, curr_node, another_node);
			array_of_trees_degrees[curr_degree] = NULL;
			curr_degree++;
		}

		if (((operation == '<' && (curr_node->key->wage < heap->root->key->wage)) || operation == '>' && (curr_node->key->wage > heap->root->key->wage)) && curr_node != heap->root) {
			heap->root = curr_node;
		}
		if (another_node != curr_node)
			array_of_trees_degrees[curr_degree] = curr_node;

		curr_node = curr_node->right;
		curr_degree = curr_node->degree;
	} while (curr_node != heap->root);

	heap->root = NULL;
	
	// make new links between members of an array
	for (i = 0; i < array_size; i++) {
		curr_node = array_of_trees_degrees[i];
		if (curr_node != NULL) {
			// start building new links
			if (heap->root == NULL) {
				heap->root = curr_node;
				heap->root->left = curr_node;
				heap->root->right = curr_node;
			}

			// new connections already appeared
			else {
				// insert a node in root array
				curr_node->left = heap->root;
				curr_node->right = heap->root->right;
				heap->root->right->left = curr_node;
				heap->root->right = curr_node;

				// refresh root if needed
				if ((operation == '<' && (curr_node->key->wage < heap->root->key->wage)) || operation == '>' && (curr_node->key->wage > heap->root->key->wage)) {
					heap->root = curr_node;
				}
			}
		}
	}

	free(array_of_trees_degrees);
	return fibonacci_extract_n_consolidate_ok;
}

fibonacci_extract_n_consolidate_statuses fibonacci_extract_root_node(char operation, fibonacci_heap* heap, heapNode** extracted)
{
	heapNode* root = heap->root;
	if (root != NULL) {
		// adds every son element of root to circular list of root
		if (root->child != NULL) {
			heapNode* curr_node = root->child, * next_node = NULL;
			do {
				next_node = curr_node->right;
				curr_node->parent = NULL;

				curr_node->left = root;
				curr_node->right = root->right;
				root->right->left = curr_node;
				root->right = curr_node;

				curr_node = next_node;
			} while (curr_node != root->child);
		}

		// delete root element from root list; now it's previous root element
		root->left->right = root->right;
		root->right->left = root->left;
		
		// if pyramid consists of 1 element
		if (root->right == root) {
			heap->root = NULL;
		}
		else {
			fibonacci_extract_n_consolidate_statuses fib_con_s = -1;

			heap->root = root->right;
			fib_con_s = fibonacci_consolidate(operation, heap);
			if (fib_con_s == fibonacci_extract_n_consolidate_malloc_error) {
				return fibonacci_extract_n_consolidate_malloc_error;
			}
		}

		heap->size--;
	}
	(*extracted) = root;
	return fibonacci_extract_n_consolidate_ok;
}

fibonacci_heap_sort_statuses fibonacci_heap_sort(char operation, employee* array, int arr_size, FILE* fi)
{
	fibonacci_heap heap;
	initialize_heap(&heap);

	int i;
	fibonacci_insert_statuses fib_ins_s = -1;
	for (i = 0; i < arr_size; i++) {
		fib_ins_s = fibonacci_insert_node(operation, &heap, &(array[i]));
		switch (fib_ins_s)
		{
		case fibonacci_insert_heap_incorrect_heap_ptr:
			return fibonacci_heap_sort_unexpected_error;
		case fibonacci_insert_malloc_error:
			return fibonacci_heap_sort_malloc_error;
		default:
			break;
		}
	}

	heapNode* extracted = NULL;
	fibonacci_extract_n_consolidate_statuses fib_extr_n_con_s = -1;
	for (i = 0; i < arr_size; i++) {
		fib_extr_n_con_s = fibonacci_extract_root_node(operation, &heap, &extracted);

		if (fib_extr_n_con_s == fibonacci_extract_n_consolidate_malloc_error) {
			return fibonacci_heap_sort_malloc_error;
		}

		print_employee(fi, *(extracted->key));
		free(extracted);
		extracted = NULL;
	}

	return fibonacci_heap_sort_ok;
}
>>>>>>> origin/main
