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

/// <summary>
/// Recursively deletes binary tree
/// </summary>
/// <param name="bin">- initially, root of binary tree</param>
void delete_binary_tree(binary_tree_node** bin);

/// <summary>
/// Recursively deletes prefix tree
/// </summary>
/// <param name="pref">- initially null(top) element of prefix tree</param>
void delete_prefix_tree(prefix_tree_node** pref);

/// <summary>
/// Initializes prefix tree, handles respondes from function read_lexeme, results with built binary tree
/// </summary>
/// <param name="stream">- stream from where to read</param>
/// <param name="my_bin_tree">- binary tree to build</param>
/// <returns>read_in_binary_tree_statuses</returns>
read_in_binary_tree_statuses read_in_binary_tree(FILE* stream, binary_tree* my_bin_tree);

void bin_tree_detour_in_depth(binary_tree_node* node, void* structure, int traverse(binary_tree_node*, void*));

typedef enum put_in_file_statuses {
	put_in_file_incorrect_ptr_to_file,
	put_in_file_tree_is_empty,
	put_in_file_ok
} put_in_file_statuses;

put_in_file_statuses put_in_file(FILE* stream, binary_tree my_tree);

/// <summary>
/// Prints BINARY tree (with its structure)
/// </summary>
/// <param name="stream">- the output stream</param>
/// <param name="tree">- binary tree to print</param>
void print_bin_tree(FILE* stream, binary_tree tree);


typedef enum find_times_met_statuses {
	find_times_met_no_such_word,
	find_times_met_ok
} find_times_met_statuses;

typedef struct times_met_str {
	char* to_find;
	size_t count_occured;
} times_met_str;

find_times_met_statuses find_times_met(binary_tree* tree, char* to_find, times_met_str* structure);

typedef enum find_n_most_often_statuses {
	find_n_most_often_tree_is_empty,
	find_n_most_often_incorrect_n_to_find,
	find_n_most_often_malloc_error,
	find_n_most_often_count_of_elements_is_less,
	find_n_most_often_incorrect_ptr_to_lexeme,
	find_n_most_often_ok
} find_n_most_often_statuses;

find_n_most_often_statuses find_n_often(binary_tree tree, char*** lexemes, unsigned int* processed, unsigned int n);

typedef enum find_min_max_statuses {
	find_min_max_tree_is_empty,
	find_min_max_ok
} find_min_max_statuses;

typedef struct find_min_max_str {
	char* max;
	char* min;
} find_min_max_str;

find_min_max_statuses find_min_max(binary_tree tree, find_min_max_str* structure);


#endif // !HEADER_8
