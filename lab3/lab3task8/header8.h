#ifndef HEADER_8
#define HEADER_8

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct binary_tree_node {
	char* lexeme;
	size_t count_occured;
	struct binary_tree_node* left;
	struct binary_tree_node* right;
} binary_tree_node;

typedef struct binary_tree {
	binary_tree_node* root;
} binary_tree;

typedef struct prefix_tree_node {
	char ch;                                 // key
	char allocated;							 // count of allocated elements in child_array
	struct prefix_tree_node** child_array;	 // array of child trees
	binary_tree_node* ptr_to_bin_node;		 // ptr to node in binary tree. NULL if no such lexeme
} prefix_tree_node;

typedef struct prefix_tree {
	prefix_tree_node* root;
} prefix_tree;

typedef enum prefix_tree_statuses {
	pref_malloc_error,
	pref_realloc_error,
	pref_ok,
} prefix_tree_statuses;

typedef enum binary_tree_statuses {
	bin_malloc_error,
	bin_ok,
} binary_tree_statuses;

typedef enum read_in_binary_tree_statuses {
	read_in_binary_tree_incorrect_ptr_to_file,
	read_in_binary_tree_malloc_error,
	read_in_binary_tree_realloc_error,
	read_in_binary_tree_ok,
} read_in_binary_tree_statuses;

typedef enum read_lexeme_statuses {
	read_lexeme_eof_reached,
	read_lexeme_malloc_error,
	read_lexeme_realloc_error,
	read_lexeme_ok
} read_lexeme_statuses;

void delete_binary_tree(binary_tree_node** bin);
void delete_prefix_tree(prefix_tree_node** pref);
read_in_binary_tree_statuses read_in_binary_tree(FILE* stream, binary_tree* my_bin_tree);

typedef enum find_min_max_statuses {
	find_min_max_tree_is_empty,
	find_min_max_ok
} find_min_max_statuses;

find_min_max_statuses find_min_max(binary_tree tree);

void bin_tree_detour_in_depth(binary_tree_node* node, void* structure, int traverse(binary_tree_node*, void*));

#endif // !HEADER_8
