#include "header5.h"
#define size_of_input 256

// TODO: div by zero hande (not works)

void delete_output_struct_arr(output_struct** arr, unsigned size)
{
	unsigned i;
	for (i = 0; i < size; i++) {
		free((*arr)[i].input);
		free((*arr)[i].output);
	}
	free(*arr);
	(*arr) = NULL;
}

int handle_responses(funcs_responses funcs_s) {
	switch (funcs_s)
	{
	case funcs_incorrect_ptr_to_str:
		printf("Incorrect ptr to output str passed. It must be NULL\n");
		return -3;
	case funcs_malloc_error:
		printf("Memory allocation error happened\n");
		return -4;
	case funcs_realloc_error:
		printf("Memory reallocation error happened\n");
		return -5;
	case funcs_totally_incorrect_expr:
		printf("Totally incorrect expression detected\n");
		return -6;
	default:
		break;
	}
	return 0;
}

funcs_responses make_error_filename(char* initial, char** error)
{
	size_t str_len = strlen(initial), i;
	if (!((*error) = (char*)malloc(sizeof(char) * (str_len + 7)))) {
		return funcs_malloc_error;
	}

	for (i = 0; i < str_len; i++) {
		if (initial[i] == '.')
			break;
	}

	char* tmp = (*error) + i;
	strcpy((*error), initial);
	strcpy(tmp, "_error");
	strcpy(tmp + 6, initial + i);
	return funcs_ok;
}

int main(int argc, char** argv)
{
	if (argc < 2) {
		printf("Incorrect count of command line arguments.\nPass text file(s) with correct arithmetic expressions\n");
		return -1;
	}
	/*
	1. open file
	2. read all strings of it in array of special struct
	3. start making output
	4. calculate one
	5. print to stdout
	6. close file
	*/

	FILE* in = NULL, * error_out = NULL;
	output_struct* output = NULL, * tmp = NULL;
	funcs_responses funcs_s = 0;

	unsigned i, j, allocated_out = 8, act_size = 0;
	int handle = 0;
	size_t file_str_length = 0;
	double res_calculus = 0;

	char* filename_error = NULL;

	for (i = 1; i < argc; i++) {
		allocated_out = 8; act_size = 0;
		if (!(in = fopen(argv[i], "r"))) {
			printf("Open file error\n");
			return -2;
		}

		if (!(output = (output_struct*)malloc(sizeof(output_struct) * allocated_out))) {
			fclose(in);
			handle = handle_responses(funcs_malloc_error);
			return handle;
		}

		// read all strings of it in array of special struct
		while (!feof(in)) {
			if (!(output[act_size].input = (char*)malloc(sizeof(char) * size_of_input))) {
				fclose(in);
				delete_output_struct_arr(&output, act_size);
				handle = handle_responses(funcs_malloc_error);
				return handle;
			}
			fgets(output[act_size].input, size_of_input, in);
			file_str_length = strlen(output[act_size].input);
			if (output[act_size].input[file_str_length - 1] == '\n')
				output[act_size].input[file_str_length - 1] = '\0';
			
			if (output[act_size].input[0] == 0) {
				if (!(output[act_size].output = (char*)malloc(sizeof(char) * 20))) {
					fclose(in);
					delete_output_struct_arr(&output, act_size);
					free(output[act_size].input);
					handle = handle_responses(funcs_malloc_error);
					return handle;
				}
				strcpy(output[act_size].output, "Empty string passed");
				output[act_size].output[19] = '\0';
				output[act_size].result = -1;
			}
			else {
				// start making output string
				output[act_size].output = NULL;
				funcs_s = shunting_yard(output[act_size].input, &(output[act_size].output));
				if ((handle = handle_responses(funcs_s)) != 0) {
					fclose(in);
					delete_output_struct_arr(&output, act_size);
					free(output[act_size].input);
					return handle;
				}
			}
			
			if (funcs_s == expr_parentheses_mismatched) {
				if (!(output[act_size].output = (char*)malloc(sizeof(char) * 23))) {
					fclose(in);
					delete_output_struct_arr(&output, act_size);
					free(output[act_size].input); 
					handle = handle_responses(funcs_malloc_error);
					return handle;
				}
				strcpy(output[act_size].output, "Parentheses mismatched");
				output[act_size].output[22] = '\0';
				output[act_size].result = -1;
			}
			else if (funcs_s == expr_unknown_token) {
				if (!(output[act_size].output = (char*)malloc(sizeof(char) * 20))) {
					fclose(in);
					delete_output_struct_arr(&output, act_size);
					free(output[act_size].input);
					handle = handle_responses(funcs_malloc_error);
					return handle;
				}
				strcpy(output[act_size].output, "Unknown token found");
				output[act_size].output[19] = '\0';
				output[act_size].result = -1;
			}
			


			// calculate one
			if (output[act_size].result != -1) {
				funcs_s = calculate_postfix_notation(output[act_size].output, &(output[act_size].result));
				if ((handle = handle_responses(funcs_s)) != 0) {
					fclose(in);
					delete_output_struct_arr(&output, act_size);
					free(output[act_size].input);
					return handle;
				}

				if (funcs_s == expr_division_by_zero) {
					free(output[act_size].output);
					if (!(output[act_size].output = (char*)malloc(sizeof(char) * 17))) {
						fclose(in);
						delete_output_struct_arr(&output, act_size);
						free(output[act_size].input);
						handle = handle_responses(funcs_malloc_error);
						return handle;
					}
					strcpy(output[act_size].output, "Division by zero");
					output[act_size].output[16] = '\0';
					output[act_size].result = -1;
				}
				else if (funcs_s == expr_insufficient_count_of_arguments) {
					free(output[act_size].output);
					if (!(output[act_size].output = (char*)malloc(sizeof(char) * 47))) {
						fclose(in);
						delete_output_struct_arr(&output, act_size);
						free(output[act_size].input);
						handle = handle_responses(funcs_malloc_error);
						return handle;
					}
					strcpy(output[act_size].output, "Insufficient number of arguments in expression");
					output[act_size].output[46] = '\0';
					output[act_size].result = -1;
				}
			}
			
			act_size++;
		}
		// print to stdout / file
		funcs_s = make_error_filename(argv[i], &(filename_error));
		if (funcs_s == funcs_malloc_error) {
			fclose(in);
			handle = handle_responses(funcs_malloc_error);
			return handle;
		}

		if (!(error_out = fopen(filename_error, "w"))) {
			printf("Open file error\n");
			return -2;
		}

		printf("===== Below are results for file %s =====\n", argv[i]);

		for (j = 0; j < act_size; j++) {
			if (!strcmp("Parentheses mismatched", output[j].output) ||
				!strcmp("Unknown token found", output[j].output) ||
				!strcmp("Division by zero", output[j].output) ||
				!strcmp("Empty string passed", output[j].output) ||
				!strcmp("Insufficient number of arguments in expression", output[j].output)) {
				fprintf(error_out, "[%u] %s\n    %s\n", j, output[j].input, output[j].output);
			}
			else {
				printf("[%u] %s\n    %s\n    %lf\n", j, output[j].input, output[j].output, output[j].result);
			}
		}
		printf("\n\n");


		delete_output_struct_arr(&output, act_size);
		free(filename_error);
		fclose(error_out);
		fclose(in);
	}


	return 0;
}
