#define _CRT_SECURE_NO_WARNINGS

#ifndef TREE_10
#include <stdio.h>
#include "stdlib.h"
#include <string.h>

typedef struct tree_node {
	char key;
	struct tree_node* first_child; // ptr to the first child, which is connected to others as linked list
	struct tree_node* parent;
	struct tree_node* next;
	struct tree_node* prev;
} tree_node;

typedef struct general_tree {
	tree_node* root;
} general_tree;

typedef enum read_n_build_n_print_trees_statuses {
	read_n_build_trees_incorrect_file_ptr,
	read_n_build_trees_malloc_error,
	read_n_build_trees_realloc_error,
	read_n_build_trees_ok
} read_n_build_n_print_trees_statuses;

read_n_build_n_print_trees_statuses read_n_build_n_print_trees(FILE* in, FILE* out);

#endif // !TREE_10