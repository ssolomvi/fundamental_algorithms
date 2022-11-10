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
	return ((ch >= 'A' && ch <= 'Z') ? ch - ' ' : ch);
}

read_lexeme_statuses read_lexeme(FILE* stream, prefix_tree* pref_tree, binary_tree* bin_tree)
{
	char ch = 0, * lexeme = NULL, * tmp = NULL;
	size_t size = 16, actual_size = 0, i, size_of_current_node_lexeme = 0;
	int compare_respond = 0;

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

	lexeme[actual_size++] = ch;

	prefix_tree_statuses pref_statuses = -1;
	prefix_tree_node* current_prefix_node = pref_tree->root, * node_with_key = NULL;
	binary_tree_node* current_bin_node = bin_tree->root;

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
				if (current_bin_node->lexeme[actual_size - 1] > ch) {
					current_bin_node = current_bin_node->left;
				}
				else if (current_bin_node->lexeme[actual_size - 1] < ch) {
					current_bin_node = current_bin_node->right;
				}
			}
			else if (size_of_current_node_lexeme < actual_size) {
				current_bin_node = current_bin_node->right;
			}
			else {
				continue;
			}
			while (current_bin_node) {    // move if needed and check all symbols till actual_size
				if ((compare_respond = compare_lexemes(current_bin_node->lexeme, lexeme, actual_size)) > 0) {
					current_bin_node = current_bin_node->right;
				}
				else if (compare_respond < 0) {
					current_bin_node = current_bin_node->left;
				}
				else {
					break;
				}
			}
		}
	} while (is_lexeme_char((ch = fgetc(stream))));

	lexeme[actual_size] = '\0';

	if (!current_bin_node) {
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
	return read_lexeme_ok;
}

void delete_binary_tree(binary_tree_node** bin)
{
	if (!(*bin))
		return;
	delete_binary_tree((*bin)->left);
	delete_binary_tree((*bin)->right);

	free((*bin)->lexeme);
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