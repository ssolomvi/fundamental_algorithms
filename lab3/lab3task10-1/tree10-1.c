#include "tree10-1.h"

void delete_tree(tree_node* node)
{
	if (!node)
		return;

	delete_tree(node->first_child);
	delete_tree(node->next);

	free(node);
}

typedef enum insert_node_statuses {
	insert_node_malloc_error,
	insert_node_ok
} insert_node_statuses;

insert_node_statuses insert_as_first_child(tree_node* parent_node, char to_insert)
{
	if (!(parent_node->first_child = (tree_node*)malloc(sizeof(tree_node)))) {
		return insert_node_malloc_error;
	}

	parent_node->first_child->key = to_insert;
	parent_node->first_child->first_child = NULL;
	parent_node->first_child->parent = parent_node;
	parent_node->first_child->next = NULL;
	parent_node->first_child->prev = NULL;

	return insert_node_ok;
}

insert_node_statuses insert_as_sibling(tree_node* sibling, char to_insert)
{
	if (!(sibling->next = (tree_node*)malloc(sizeof(tree_node)))) {
		return insert_node_malloc_error;
	}

	sibling->next->key = to_insert;
	sibling->next->first_child = NULL;
	sibling->next->parent = sibling->parent;
	sibling->next->next = NULL;
	sibling->next->prev = sibling;

	return insert_node_ok;
}

typedef enum build_general_tree_statuses {
	build_general_tree_malloc_error,
	build_general_tree_ok
} build_general_tree_statuses;

build_general_tree_statuses build_general_tree(char* bracket_expr, general_tree* tree)
{
	size_t len_bracket_expr = strlen(bracket_expr), i = 1;
	char curr_ch = 0;
	tree_node* curr_node = NULL;
	insert_node_statuses insert_node_s = -1;

	if (!(tree->root = (tree_node*)malloc(sizeof(tree_node)))) {
		return build_general_tree_malloc_error;
	}

	tree->root->key = *bracket_expr;
	tree->root->first_child = NULL;
	tree->root->parent = NULL;
	tree->root->next = NULL;
	tree->root->prev = NULL;

	curr_node = tree->root;

	for (i; i < len_bracket_expr; i++) {
		curr_ch = bracket_expr[i];
		switch (curr_ch)
		{
		case '(':
			if ((insert_node_s = insert_as_first_child(curr_node, bracket_expr[++i])) == insert_node_malloc_error) {
				delete_tree(tree->root);
				tree->root = NULL;
				return build_general_tree_malloc_error;
			}

			curr_node = curr_node->first_child;
			break;
		case ',':
			if ((insert_node_s = insert_as_sibling(curr_node, bracket_expr[++i])) == insert_node_malloc_error) {
				delete_tree(tree->root);
				tree->root = NULL;
				return build_general_tree_malloc_error;
			}

			curr_node = curr_node->next;
			break;
		case ')':
			curr_node = curr_node->parent;
			break;
		default:
			break;
		}
	}

	return build_general_tree_ok;
}

int read_string(FILE* stream, char** new_str, int* file_ended)
{
	size_t allocated = 16, act_size = 0;
	char* tmp = NULL, ch = 0;
	if (!((*new_str) = (char*)malloc(sizeof(char) * allocated))) {
		return -1;
	}

	while ((ch = fgetc(stream)) != '\n' && !feof(stream)) {
		if (act_size >= allocated - 1) {
			allocated *= 2;
			if (!(tmp = (char*)realloc((*new_str), sizeof(char) * allocated))) {
				free(*new_str);
				return -2;
			}
			(*new_str) = tmp;
		}

		(*new_str)[act_size++] = ch;
	}

	if (!(tmp = (char*)realloc((*new_str), sizeof(char) * (act_size + 1)))) {
		free(*new_str);
		return -2;
	}
	(*new_str) = tmp;
	(*new_str)[act_size] = '\0';

	if (ch == EOF || feof(stream)) {
		(*file_ended) = 1;
	}

	return 0;
}

void print_node(FILE* stream, tree_node* curr_node, unsigned int curr_depth)
{
	if (!curr_node)
		return;

	if (curr_node->next) {
		print_node(stream, curr_node->next, curr_depth);
	}

	if (curr_node->first_child) {
		print_node(stream, curr_node->first_child, curr_depth + 1);
	}

	unsigned int i;
	for (i = 0; i < curr_depth; i++) {
		fprintf(stream, "      ");
	}

	if (curr_node->first_child) {
		fprintf(stream, " %c -> \n", curr_node->key);
	}
	else {
		fprintf(stream, " %c    \n", curr_node->key);
	}
}

int print_general_tree(FILE* stream, general_tree tree, char* bracket_expr)
{
	unsigned int curr_depth = 0;

	fprintf(stream, "=================================================\n");
	fprintf(stream, "Below if the general tree for bracket expression:\n%s\n\n", bracket_expr);
	print_node(stream, tree.root, 0);
	fprintf(stream, "\n");
	fprintf(stream, "=================================================\n\n\n");

	return  0;
}

read_n_build_n_print_trees_statuses read_n_build_n_print_trees(FILE* in, FILE* out)
{
	if (!in || !out)
		return read_n_build_trees_incorrect_file_ptr;

	int file_ended = 0, read_string_response = 0;
	char* bracket_expr = NULL;
	general_tree tree;
	tree.root = NULL;
	build_general_tree_statuses build_general_tree_s = -1;

	while (!file_ended) {
		// read string
		// build general tree
		// print tree
		// delete tree and string
		// repeat

		if ((read_string_response = read_string(in, &bracket_expr, &file_ended)) != 0) {
			if (read_string_response == -1)
				return read_n_build_trees_malloc_error;
			if (read_string_response == -2)
				return read_n_build_trees_realloc_error;
		}

		if (strlen(bracket_expr) == 0) {
			free(bracket_expr);
			break;
		}

		if ((build_general_tree_s = build_general_tree(bracket_expr, &tree)) != build_general_tree_ok) {
			free(bracket_expr);
			return read_n_build_trees_malloc_error;
		}

		print_general_tree(out, tree, bracket_expr);

		free(bracket_expr);
		bracket_expr = NULL;
		delete_tree(tree.root);
		tree.root = NULL;
	}

	return read_n_build_trees_ok;
}