#define _CRT_SECURE_NO_WARNINGS

#ifndef HEADER_5
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define is_ident(c) (((c) >= '0' && (c) <= '9'))
#define is_operator(c) ((c) == '+' || (c) == '-' || (c) == '/' || (c) == '*' || (c) == '%' || (c) == '^')

typedef enum funcs_responses {
	expr_parentheses_mismatched,
	expr_unknown_token,
	expr_division_by_zero,
	expr_insufficient_count_of_arguments,
	funcs_incorrect_ptr_to_str,
	funcs_malloc_error,
	funcs_realloc_error,
	funcs_totally_incorrect_expr,
	funcs_ok
} funcs_responses;

typedef struct lin_list_elem {
	char data;
	struct lin_list_elem* next;
} lin_list_elem; 

typedef struct stack_struct {
	lin_list_elem* top;
	size_t count_of_elements;
} stack_struct;

typedef struct lin_list_elem_double {
	double data;
	struct lin_list_elem_double* next;
} lin_list_elem_double;

typedef struct stack_struct_double {
	size_t count_of_elements;
	lin_list_elem_double* top;
} stack_struct_double;

typedef struct output_struct {
	char* input, * output;
	double result;
} output_struct;

funcs_responses shunting_yard(const char* input, char** output);
funcs_responses calculate_postfix_notation(char* str, double* res);

#endif // !HEADER_5
