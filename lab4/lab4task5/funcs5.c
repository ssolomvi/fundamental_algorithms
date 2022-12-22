#include "header5.h"

int op_prior(const char c)
{
    switch (c)
    {
    case '^':
        return 3;

    case '*':
    case '/':
    case '%':
        return 2;

    case '+':
    case '-':
        return 1;
    }
    return 0;
}

bool op_left_assoc(const char c)
{
    switch (c)
    {
    case '*':
    case '/':
    case '%':
    case '+':
    case '-':
    case '^':
        return true;
    }
    return false;
}

unsigned int op_arg_count(const char c)
{
    switch (c)
    {
    case '*':
    case '/':
    case '%':
    case '+':
    case '-':
    case '^':
        return 2;

    default:
        return c - 'A';
    }
    return 0;
}

funcs_responses push_stack(stack_struct* stack, char ch)
{
    lin_list_elem* new_elem = (lin_list_elem*)malloc(sizeof(lin_list_elem));

    if (!new_elem)
        return funcs_realloc_error;

    new_elem->data = ch;

    if (!(stack->count_of_elements)) {
        stack->top = new_elem;
        stack->top->next = NULL;
        stack->count_of_elements = 1;
    }
    else {
        new_elem->next = stack->top;
        stack->top = new_elem;
        stack->count_of_elements++;
    }

    return funcs_ok;
}

void pop_stack(stack_struct* stack)
{
    if (!(stack->count_of_elements))
        return;

    lin_list_elem* tmp = stack->top;
    stack->top = stack->top->next;
    free(tmp);
    stack->count_of_elements--;
}

void delete_stack(stack_struct* stack)
{
    while (stack->count_of_elements) {
        pop_stack(stack);
    }
}

funcs_responses shunting_yard(const char* input, char** output)
{
    if ((*output))
        return funcs_incorrect_ptr_to_str;

    char* input_cur_pos = input, * input_end = input + strlen(input);
    unsigned allocated_out = 16, act_size_out = 0;

    if (!((*output) = (char*)malloc(sizeof(char) * allocated_out))) {
        return funcs_malloc_error;
    }

    char curr_ch = 0, prev_ch = 0, stack_top_op = 0, * tmp = NULL;
    bool left_bracket_appeared = false;

    stack_struct stack;
    stack.top = NULL; stack.count_of_elements = 0;
    funcs_responses funcs_re = -1;

    while (input_cur_pos < input_end) {
        curr_ch = *input_cur_pos;

        if (curr_ch != ' ') {
            if (is_ident(curr_ch)) {
                if (allocated_out <= act_size_out) {
                    allocated_out *= 2;
                    if (!(tmp = (char*)realloc((*output), sizeof(char) * allocated_out))) {
                        free(*output);
                        return funcs_realloc_error;
                    }
                    (*output) = tmp;
                }
                (*output)[act_size_out++] = curr_ch;
            }

            else if (is_operator(curr_ch)) {
                if (is_ident(prev_ch)) {
                    if (allocated_out <= act_size_out) {
                        allocated_out *= 2;
                        if (!(tmp = (char*)realloc((*output), sizeof(char) * allocated_out))) {
                            free(*output);
                            delete_stack(&stack);
                            return funcs_realloc_error;
                        }
                        (*output) = tmp;
                    }
                    (*output)[act_size_out++] = ' ';
                }
                while (stack.count_of_elements > 0) {
                    stack_top_op = stack.top->data;

                    if (is_operator(stack_top_op) &&
                        ((op_left_assoc(curr_ch) && (op_prior(curr_ch) <= op_prior(stack_top_op))) ||
                            (!op_left_assoc(curr_ch) && (op_prior(curr_ch) < op_prior(stack_top_op))))) {
                        if (allocated_out <= act_size_out) {
                            allocated_out *= 2;
                            if (!(tmp = (char*)realloc((*output), sizeof(char) * allocated_out))) {
                                free(*output);
                                delete_stack(&stack);
                                return funcs_realloc_error;
                            }
                            (*output) = tmp;
                        }
                        (*output)[act_size_out++] = stack.top->data;
                        pop_stack(&stack);
                    }
                    else {
                        break;
                    }
                }
                if ((funcs_re = push_stack(&stack, curr_ch)) != funcs_ok) {
                    free(*output);
                    delete_stack(&stack);
                    return funcs_re;
                }
            }

            else if (curr_ch == '(') {
                if ((funcs_re = push_stack(&stack, curr_ch)) != funcs_ok) {
                    free(*output);
                    delete_stack(&stack);
                    return funcs_re;
                }
            }

            else if (curr_ch == ')') {
                left_bracket_appeared = false;

                while (stack.count_of_elements > 0) {
                    stack_top_op = stack.top->data;

                    if (stack_top_op == '(') {
                        left_bracket_appeared = true;
                        pop_stack(&stack);
                        break;
                    }
                    else {
                        if (allocated_out <= act_size_out) {
                            allocated_out *= 2;
                            if (!(tmp = (char*)realloc((*output), sizeof(char) * allocated_out))) {
                                free(*output);
                                delete_stack(&stack);
                                return funcs_realloc_error;
                            }
                            (*output) = tmp;
                        }
                        (*output)[act_size_out++] = stack.top->data;

                        pop_stack(&stack);
                    }
                }

                if (!left_bracket_appeared) {
                    free(*output);
                    delete_stack(&stack);
                    return expr_parentheses_mismatched;
                }
            }

            else {
                return expr_unknown_token;
            }
        }
        if (is_ident(prev_ch) && curr_ch == ' ') {
            if (allocated_out <= act_size_out) {
                allocated_out *= 2;
                if (!(tmp = (char*)realloc((*output), sizeof(char) * allocated_out))) {
                    free(*output);
                    delete_stack(&stack);
                    return funcs_realloc_error;
                }
                (*output) = tmp;
            }
            (*output)[act_size_out++] = curr_ch;
        }

        prev_ch = curr_ch;
        ++input_cur_pos;
    }

    while (stack.count_of_elements > 0) {
        stack_top_op = stack.top->data;

        if (stack_top_op == '(' || stack_top_op == ')') {
            free(*output);
            delete_stack(&stack);
            return expr_parentheses_mismatched;
        }

        if (allocated_out <= act_size_out) {
            allocated_out *= 2;
            if (!(tmp = (char*)realloc((*output), sizeof(char) * allocated_out))) {
                free(*output);
                delete_stack(&stack);
                return funcs_realloc_error;
            }
            (*output) = tmp;
        }
        (*output)[act_size_out++] = stack.top->data;
        pop_stack(&stack);
    }

    if (!(tmp = (char*)realloc((*output), sizeof(char) * (act_size_out + 1)))) {
        free(*output);
        delete_stack(&stack);
        return funcs_realloc_error;
    }
    (*output) = tmp;
    (*output)[act_size_out] = 0;

    return funcs_ok;
}

funcs_responses push_in_double_stack(stack_struct_double* stack, double data)
{
    lin_list_elem_double* new_elem = (lin_list_elem_double*)malloc(sizeof(lin_list_elem_double));
    if (!new_elem)
        return funcs_malloc_error;
    new_elem->data = data;

    if (!(stack->top)) {
        new_elem->next = NULL;
        stack->top = new_elem;
        stack->count_of_elements = 1;
    }
    else {
        new_elem->next = stack->top;
        stack->top = new_elem;
        stack->count_of_elements++;
    }
    return funcs_ok;
}

double pop_from_double_stack(stack_struct_double* stack)
{
    if (!stack->top)
        return 0;

    double to_return = stack->top->data;
    lin_list_elem_double* tmp = stack->top;
    stack->top = stack->top->next;
    free(tmp);
    stack->count_of_elements--;
    return to_return;
}

void delete_double_stack(stack_struct_double* stack)
{
    if (!stack->top)
        return 0;

    lin_list_elem_double* ptr = stack->top, * tmp = NULL;
    while (ptr) {
        tmp = ptr;
        ptr = ptr->next;
        free(tmp);
    }
    stack->top = NULL;
    stack->count_of_elements = 0;
}

double to_num(char ch)
{
    return ((ch >= '0' && ch <= '9') ? ch - '0' : ch);
}

double perform_operation(char op, double fi, double se)
{
    switch (op)
    {
    case '*':
        return fi * se;
    case '/':
        return fi / se;
    case '%':
        return ((long)fi) % ((long)se);
    case '+':
        return fi + se;
    case '-':
        return fi - se;
    case '^':
        return pow(fi, se);
    }
    return 0;
}

funcs_responses calculate_postfix_notation(char* str, double* res)
{
    (*res) = 0;
    size_t i, str_len = strlen(str);
    stack_struct_double stack;
    stack.top = NULL;
    funcs_responses funcs_s = -1;
    char ch = 0, prev_ch = 0;
    double number = 0, mult = 1, fi = 0, se = 0;
    bool met_comma = false;
    unsigned count_of_args = 0;

    for (i = 0; i < str_len; i++) {
        ch = str[i];
        if (is_ident(ch)) {
            if (prev_ch == ',' || prev_ch == '.') {
                met_comma = true;
            }
            if (met_comma) {
                mult /= 10;
                number += to_num(ch) * mult;
            }
            if (!met_comma) {
                number *= 10;
                number += to_num(ch);
            }
        }
        else {
            if (is_ident(prev_ch)) {
                funcs_s = push_in_double_stack(&stack, number);
                if (funcs_s == funcs_malloc_error) {
                    delete_double_stack(&stack);
                    return funcs_s;
                }
                number = 0;
                mult = 1;
            }
            if (is_operator(ch)) {
                count_of_args = op_arg_count(ch);
                if (stack.count_of_elements < count_of_args) {
                    delete_double_stack(&stack);
                    return expr_insufficient_count_of_arguments;
                }

                se = pop_from_double_stack(&stack);
                fi = pop_from_double_stack(&stack);

                if (ch == '/' && se == 0) {
                    delete_double_stack(&stack);
                    return expr_division_by_zero;
                }

                funcs_s = push_in_double_stack(&stack,
                    perform_operation(ch, fi, se));
                if (funcs_s == funcs_malloc_error) {
                    delete_double_stack(&stack);
                    return funcs_s;
                }
            }
        }
        
        prev_ch = ch;
    }
    if (!stack.top) {
        delete_double_stack(&stack);
        return funcs_totally_incorrect_expr;
    }
    (*res) = stack.top->data;
    delete_double_stack(&stack);
    return funcs_ok;
}