#include "header8.h"
#include <math.h>

int is_lexeme_char(char ch)
{
	return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '-'));
}

int is_first_letter_in_lexeme(char ch)
{
	return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
}

void init_prefix_tree_node(prefix_tree_node** init)
{
	(*init)->allocated = 0;
	(*init)->ch = 0;
	(*init)->child_array = NULL;
	(*init)->ptr_to_bin_node = NULL;
}

void init_binary_tree_node(binary_tree_node** init)
{
	(*init)->count_occured = 0;
	(*init)->left = NULL;
	(*init)->right = NULL;
	// (*init)->lexeme = NULL;
}

prefix_tree_statuses init_prefix_tree(prefix_tree* pref_tree)
{
	if (!(pref_tree->root = (prefix_tree_node*)malloc(sizeof(prefix_tree_node)))) {
		return pref_malloc_error;
	}

	pref_tree->root->ch = '\0';
	pref_tree->root->child_array = NULL;
	pref_tree->root->ptr_to_bin_node = NULL;
	pref_tree->root->allocated = 0;
	return pref_ok;
}

void bin_search_ind(prefix_tree_node** array, char count, char to_find, char* index, char* nearest)
{
	(*nearest) = -1;
	char start = 0, end = count - 1, middle = 0, current_ch = 0;
	while (start <= end) {
		middle = floor((start + (float)end) / 2);
		if ((current_ch = array[middle]->ch) < to_find) {
			start = middle + 1;
			(*nearest) = middle;
		}
		else if (current_ch > to_find) {
			end = middle - 1;
		}
		else {
			(*index) = middle;
			return;
		}
	}
	(*index) = -1;
}

void move_prefixe_node(prefix_tree_node** old_place, prefix_tree_node** new_place)
{
	(*new_place)->allocated = (*old_place)->allocated;
	(*new_place)->ch = (*old_place)->ch;
	(*new_place)->child_array = (*old_place)->child_array;
	(*new_place)->ptr_to_bin_node = (*old_place)->ptr_to_bin_node;
}

prefix_tree_statuses insert_into_prefix_tree(prefix_tree_node** insert_after, char key, prefix_tree_node** node_with_key)
{
	char index_in_array = -1, nearest = -1;
	prefix_tree_node** tmp = NULL;
	int i;

	// memory for prefix tree root MUST be allocated, but not the memory for its children
#pragma region allocate memory in child_array
	if (!((*insert_after)->child_array)) {
		if (!((*insert_after)->child_array = (prefix_tree_node**)malloc(sizeof(prefix_tree_node*)))) {
			return pref_malloc_error;
		}
	}
	else {
		bin_search_ind((*insert_after)->child_array, (*insert_after)->allocated, key, &index_in_array, &nearest);
		if (index_in_array != -1) {  // such value is already in an array
			(*node_with_key) = (*insert_after)->child_array[index_in_array];
			return pref_ok;
		}
		if (!(tmp = (prefix_tree_node**)realloc((*insert_after)->child_array,
			sizeof(prefix_tree_node*) * ((*insert_after)->allocated + 1)))) {
			return pref_realloc_error;
		}
		(*insert_after)->child_array = tmp;
	}
#pragma endregion

	if (!((*insert_after)->child_array[(*insert_after)->allocated] = (prefix_tree_node*)malloc(sizeof(prefix_tree_node)))) {
		return pref_malloc_error;
	}

	if (!(*insert_after)->allocated) {
		init_prefix_tree_node(&((*insert_after)->child_array[((*insert_after)->allocated)]));
		(*insert_after)->child_array[(*insert_after)->allocated]->ch = key;
	}
	else {
		for (i = (*insert_after)->allocated - 1; i > nearest; i--) {
			move_prefixe_node(&((*insert_after)->child_array[i]), &((*insert_after)->child_array[i + 1]));
		}
		init_prefix_tree_node(&((*insert_after)->child_array[nearest + 1]));
		(*insert_after)->child_array[nearest + 1]->ch = key;
	}
	(*node_with_key) = (*insert_after)->child_array[nearest + 1];
	(*insert_after)->allocated++;
	return pref_ok;
}

int compare_lexemes(char* first, char* second, size_t second_size)
{
	size_t start = 0, len_first = strlen(first);
	if (len_first < second_size) {
		for (start; start < len_first; start++) {
			if (first[start] > second[start])
				return -1;
			if (first[start] < second[start])
				return 1;
		}
		return 1;
	}
	else {
		for (start; start < second_size; start++) {
			if (first[start] > second[start])
				return -1;
			if (first[start] < second[start])
				return 1;
		}
	}
	return 0;
}

char to_lower(char ch)
{
	return ((ch >= 'A' && ch <= 'Z') ? ch + ' ' : ch);
}

/// <summary>
/// Function which reads lexeme from file and while reading forms prefix tree and finds place to insert a (new) node in binary tree
/// </summary>
/// <param name="stream">- stream from where to read</param>
/// <param name="pref_tree">- prefix tree to build / append</param>
/// <param name="bin_tree">- binary tree to build / append</param>
/// <returns>read_lexeme_statuses</returns>
read_lexeme_statuses read_lexeme(FILE* stream, prefix_tree* pref_tree, binary_tree* bin_tree)
{
	char ch = 0, * lexeme = NULL, * tmp = NULL;
	size_t size = 16, actual_size = 0, size_of_current_node_lexeme = 0;
	int compare_respond = 0, turn = 0; // turn is a flag, showing which subtree in bintree is a subtree to insert new lexeme

	if (!(lexeme = (char*)malloc(sizeof(char) * size))) {
		delete_binary_tree(&(bin_tree->root));
		delete_prefix_tree(&(pref_tree->root));
		return read_lexeme_malloc_error;
	}

	while (!is_first_letter_in_lexeme((ch = fgetc(stream))) && ch != EOF);

	if (!is_first_letter_in_lexeme(ch)) {
		free(lexeme);
		return read_lexeme_eof_reached;
	}

	prefix_tree_statuses pref_statuses = -1;
	prefix_tree_node* current_prefix_node = pref_tree->root, * node_with_key = NULL;
	binary_tree_node* current_bin_node = bin_tree->root, *parent_to_current_node = NULL;

	/*
	if such words does not exist, find parent node (after which insert)
	*/

	if (bin_tree->root)
		size_of_current_node_lexeme = strlen(current_bin_node->lexeme);

	do {
		if (actual_size >= size - 1) {
			size *= 2;
			if (!(tmp = (char*)realloc(lexeme, sizeof(char) * size))) {
				delete_binary_tree(&(bin_tree->root));
				delete_prefix_tree(&(pref_tree->root));
				free(lexeme);
				return read_lexeme_realloc_error;
			}
			lexeme = tmp;
		}

		// make a new node in the prefix tree is needed, get address of a tree with last read symbol as a key
		pref_statuses = insert_into_prefix_tree(&current_prefix_node, to_lower(ch), &node_with_key);
		if (pref_statuses != pref_ok) {
			delete_binary_tree(&(bin_tree->root));
			delete_prefix_tree(&(pref_tree->root));
			free(lexeme);
			switch (pref_statuses)
			{
			case pref_malloc_error:
				return read_lexeme_malloc_error;
			case pref_realloc_error:
				return read_lexeme_realloc_error;
			}
		}

		current_prefix_node = node_with_key;
		node_with_key = NULL;
		lexeme[actual_size++] = ch;

		// we start with binary tree uninitialized (root IS nullptr)
		if (current_bin_node) {
			if ((size_of_current_node_lexeme >= actual_size) && (current_bin_node->lexeme[actual_size - 1] != ch)) {    // if they are equal, there is no need to make a new node
				parent_to_current_node = current_bin_node;
				if (current_bin_node->lexeme[actual_size - 1] > ch) {
					current_bin_node = current_bin_node->left;
					turn = -1;
				}
				else if (current_bin_node->lexeme[actual_size - 1] < ch) {
					current_bin_node = current_bin_node->right;
					turn = 1;
				}
			}
			else if (size_of_current_node_lexeme < actual_size) {
				parent_to_current_node = current_bin_node;
				current_bin_node = current_bin_node->right;
				turn = 1;
			}
			else {
				continue;
			}
			while (current_bin_node) {    // move if needed and check all symbols till actual_size
				if ((compare_respond = compare_lexemes(current_bin_node->lexeme, lexeme, actual_size)) > 0) {
					parent_to_current_node = current_bin_node;
					current_bin_node = current_bin_node->right;
					turn = 1;
				}
				else if (compare_respond < 0) {
					parent_to_current_node = current_bin_node;
					current_bin_node = current_bin_node->left;	
					turn = -1;
				}
				else {
					size_of_current_node_lexeme = strlen(current_bin_node->lexeme);
					parent_to_current_node = NULL;
					turn = 0;
					break;
				}
			}
		}
	} while (is_lexeme_char((ch = fgetc(stream))));

	lexeme[actual_size] = '\0';

	// Finding place for insertion in biinary tree (or updating count of occurance)
	if (!(*bin_tree).root) {
		if (!((*bin_tree).root = (binary_tree_node*)malloc(sizeof(binary_tree_node)))) {
			delete_binary_tree(&(bin_tree->root));
			delete_prefix_tree(&(pref_tree->root));
			free(lexeme);
			return read_lexeme_malloc_error;
		}
		if (!(bin_tree->root->lexeme = (char*)malloc(sizeof(char) * (actual_size + 1)))) {
			delete_binary_tree(&(bin_tree->root));
			delete_prefix_tree(&(pref_tree->root));
			free(lexeme);
			free(current_bin_node);
			return read_lexeme_malloc_error;
		}
		init_binary_tree_node(&(bin_tree->root));
		strcpy(bin_tree->root->lexeme, lexeme);
		current_prefix_node->ptr_to_bin_node = current_bin_node;
		bin_tree->root->count_occured++;
	}
	else if (!current_bin_node) {
		if (!(current_bin_node = (binary_tree_node*)malloc(sizeof(binary_tree_node)))) {
			delete_binary_tree(&(bin_tree->root));
			delete_prefix_tree(&(pref_tree->root));
			free(lexeme);
			return read_lexeme_malloc_error;
		}
		if (!(current_bin_node->lexeme = (char*)malloc(sizeof(char) * (actual_size + 1)))) {
			delete_binary_tree(&(bin_tree->root));
			delete_prefix_tree(&(pref_tree->root));
			free(lexeme);
			free(current_bin_node);
			return read_lexeme_malloc_error;
		}
		init_binary_tree_node(&current_bin_node);
		strcpy(current_bin_node->lexeme, lexeme);
		current_prefix_node->ptr_to_bin_node = current_bin_node;
		current_bin_node->count_occured++;
		if (turn > 0) {
			parent_to_current_node->right = current_bin_node;
		}
		else if (turn < 0) {
			parent_to_current_node->left = current_bin_node;
		}
	}
	else {
		current_bin_node->count_occured++;
	}

	free(lexeme);
	return read_lexeme_ok;
}

read_in_binary_tree_statuses read_in_binary_tree(FILE* stream, binary_tree* my_bin_tree)
{
	if (!stream)
		return read_in_binary_tree_incorrect_ptr_to_file;

	prefix_tree my_prefix_tree;
	prefix_tree_statuses prefix_tree_s = -1;
	read_lexeme_statuses read_lexeme_s = -1;

	prefix_tree_s = init_prefix_tree(&my_prefix_tree);
	if (prefix_tree_s == pref_malloc_error) {
		return read_in_binary_tree_malloc_error;
	}

	while (!feof(stream)) {
		read_lexeme_s = read_lexeme(stream, &my_prefix_tree, my_bin_tree);
		switch (read_lexeme_s)
		{
		case read_lexeme_malloc_error:
			return read_in_binary_tree_malloc_error;
		case read_lexeme_realloc_error:
			return read_in_binary_tree_realloc_error;
		default:
			break;
		}
	}
	delete_prefix_tree(&(my_prefix_tree.root));
	return read_lexeme_ok;
}

void delete_binary_tree(binary_tree_node** bin)
{
	if ((*bin) == NULL)
		return;
	delete_binary_tree(&((*bin)->left));
	delete_binary_tree(&((*bin)->right));

	free((*bin)->lexeme);
	free(*bin);
}

void delete_prefix_tree(prefix_tree_node** pref)
{
	if (!(*pref))
		return;
	char i;
	if ((*pref)->child_array) {
		for (i = 0; i < (*pref)->allocated; i++) {
			delete_prefix_tree(&((*pref)->child_array)[i]);
		}
		free((*pref)->child_array);
	}
	free(*pref);
}

void bin_tree_detour_in_depth(binary_tree_node* node, void* structure, int traverse(binary_tree_node*, void*))
{
	if (!node || !traverse(node, structure))
		return;
	bin_tree_detour_in_depth(node->left, structure, traverse);
	bin_tree_detour_in_depth(node->right, structure, traverse);
}

# pragma region find first n elements the most often used
typedef struct lin_list_elem {
	binary_tree_node* node;
	struct lin_list_elem* next;
} lin_list_elem;

typedef struct lin_list {
	lin_list_elem* top;
	unsigned count;
} lin_list;

typedef struct often_used_str {
	int n, processed;  // processed -- count of processed nodes
	lin_list list;
} often_used_str;

int define_most_often(binary_tree_node* node, void* struc)
{
	if (!node)
		return;
	often_used_str* my_struct = ((often_used_str*)struc);
	lin_list_elem* tmp = NULL, *new_elem = NULL;

	// if structure is not fullfilled
	if (my_struct->n > my_struct->processed) {
		if (!(new_elem = (lin_list_elem*)malloc(sizeof(lin_list_elem)))) {
			return 0;
		}
		new_elem->node = node;

		my_struct->list.count = 1;

		if (!my_struct->list.top) {
			my_struct->list.top = new_elem;
			my_struct->list.top->next = NULL;
			return 1;
		}

		tmp = my_struct->list.top;

		while (tmp->next != NULL) {
			tmp = tmp->next;
		}

		tmp->next = new_elem;
		new_elem->next = NULL;
		my_struct->list.count++;
	}
	// if structure is filled
	else {
		tmp = my_struct->list.top;
		while (tmp) {
			if (tmp->node->count_occured < node->count_occured) {
				tmp->node = node;
				return 1;
			}
			tmp = tmp->next;
		}
	}
	my_struct->processed++;
}

void delete_lin_list(lin_list* list)
{
	lin_list_elem* tmp = NULL, * ptr = list->top;
	
	while (ptr) {
		tmp = ptr;
		ptr = ptr->next;
		free(tmp);
	}
}

find_n_most_often_statuses find_n_often(binary_tree tree, char*** lexemes, unsigned int* processed, unsigned int n)
{
	if (!tree.root)
		return find_n_most_often_tree_is_empty;
	
	often_used_str my_struct;
	my_struct.list.top = NULL;
	my_struct.list.count = 0;
	my_struct.n = n;
	my_struct.processed = 0;

	bin_tree_detour_in_depth(tree.root, &my_struct, define_most_often);

	if (my_struct.processed < n) {
		delete_lin_list(&(my_struct.list));
		(*processed) = my_struct.processed;
		return find_n_most_often_count_of_elements_is_less;
	}

	if (!((*lexemes) = (char**)malloc(sizeof(char*) * n))) {
		return find_n_most_often_malloc_error;
	}

	unsigned i;
	lin_list_elem* tmp = my_struct.list.top;

	for (i = 0; i < n && tmp != NULL; i++) {
		(*lexemes)[i] = tmp->node->lexeme;
		tmp = tmp->next;
	}
	delete_lin_list(&(my_struct.list));

	return find_n_most_often_ok;
}
#pragma endregion

# pragma region find min and max lexemes in tree
int compare_lexemes_min_max(char* lex1, char* lex2)
{
	return strcmp(lex1, lex2);
}

int define_min_max(binary_tree_node* node, void* structure)
{
	find_min_max_str* my_str = (find_min_max_str*)structure;
	if (!my_str->max) {
		my_str->max = node->lexeme;
		my_str->min = node->lexeme;
	}
	else {
		if (compare_lexemes_min_max(my_str->max, node->lexeme) < 0) {
			my_str->max = node->lexeme;
		}
		if (compare_lexemes_min_max(my_str->min, node->lexeme) > 0) {
			my_str->min = node->lexeme;
		}
	}
	return 1;
}

find_min_max_statuses find_min_max(binary_tree tree, find_min_max_str* structure)
{
	structure->max = NULL;
	structure->min = NULL;

	bin_tree_detour_in_depth(tree.root, structure, define_min_max);

	if (!structure->max)
		return find_min_max_tree_is_empty;

	return find_min_max_ok;
}

# pragma endregion

# pragma region find how many times a certain lexeme occures
int define_times_met(binary_tree_node* node, void* structure)
{
	times_met_str* my_str = (times_met_str*)structure;

	if (!strcmp(node->lexeme, ((char*)my_str->to_find))) {
		my_str->count_occured = node->count_occured;
		return 1;
	}
	return -1;
}

find_times_met_statuses find_times_met(binary_tree* tree, char* to_find, times_met_str* structure)
{
	if (!tree->root)
		return find_times_met_no_such_word;
	structure->count_occured = 0;
	structure->to_find = to_find;

	bin_tree_detour_in_depth(tree->root, structure, define_times_met);
	if (structure->count_occured == 0)
		return find_times_met_no_such_word;
	return find_times_met_ok;
}

# pragma endregion

int print_lexemas(binary_tree_node* node, void* stream)
{
	FILE* my_file_ptr = ((FILE*)stream);
	unsigned i;
	for (i = 0; i < node->count_occured; i++) {
		fprintf(my_file_ptr, "%s ", node->lexeme);
	}
	return 1;
}

put_in_file_statuses put_in_file(FILE* stream, binary_tree my_tree)
{
	if (!stream)
		return put_in_file_incorrect_ptr_to_file;
	if (!my_tree.root)
		return put_in_file_tree_is_empty;

	bin_tree_detour_in_depth(my_tree.root, stream, print_lexemas);
	fprintf(stream, "\n");
	return put_in_file_ok;
}

typedef struct trunk {
	struct trunk* prev;
	char* lexeme;
} trunk;

void show_trunks(FILE* stream, trunk* tr)
{
	if (!tr)
		return;
	show_trunks(stream, tr->prev);
	fprintf(stream, "%s", tr->lexeme);
}

void print_bin_tree_node(FILE* stream, binary_tree_node* root, trunk* prev, char isLeft)
{
	if (!root)
		return;
	char* prev_str = "       ";
	trunk* new_trunk = (trunk*)malloc(sizeof(trunk)); 
	if (!new_trunk)
		return;
	new_trunk->lexeme = (char*)malloc(sizeof(char) * 8);
	if (!new_trunk->lexeme) {
		free(new_trunk);
		return;
	}
	
	new_trunk->prev = prev;
	if (prev)
		strcpy(new_trunk->lexeme, prev->lexeme);
	else {
		strcpy(new_trunk->lexeme, prev_str);
	}
	
	print_bin_tree_node(stream, root->right, new_trunk, 1);

	if (!prev) {
		strcpy(new_trunk->lexeme, "-------");
	}
	else if (isLeft) {
		strcpy(new_trunk->lexeme, ".------");
		prev_str = "      |";
	}
	else {
		strcpy(new_trunk->lexeme, "'------");
		strcpy(prev->lexeme, prev_str);
	}

	show_trunks(stream, new_trunk);
	fprintf(stream, " %s (%Iu)\n", root->lexeme, root->count_occured);

	if (prev) {
		strcpy(prev->lexeme, prev_str);
	}
	strcpy(new_trunk->lexeme, "      |");
	print_bin_tree_node(stream, root->left, new_trunk, 0);
	free(new_trunk->lexeme);
	free(new_trunk);
}

void print_bin_tree(FILE* stream, binary_tree tree)
{
	print_bin_tree_node(stream, tree.root, NULL, 0);
}
