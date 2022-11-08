#pragma once
#ifndef HEADER_9
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum gen_matrix_statuses {
	gen_malloc_error,
	gen_ok
} gen_matrix_statuses;

typedef enum mult_matrixes_statuses {
	mult_matrixes_malloc_error,
	mult_matrixes_incorrect_matrixes,
	mult_matrixes_ok,
} mult_matrixes_statuses;

typedef enum matrix_deter_statuses {
	matrix_deter_incorrect_matrix,
	matrix_deter_ok
} matrix_deter_statuses;

/// <summary>
/// Prints matrix.
/// </summary>
/// <param name="matrix"></param>
/// <param name="strs"> - number of strings of matrix </param>
/// <param name="cols"> - number of columns of matrix </param>
void pr_matrix(float** matrix, int strs, int cols);

/// <summary>
/// Generates matrix with randomized values [-100; 100]
/// </summary>
/// <param name="matrix"> - matrix to generate</param>
/// <param name="strs"> - number of strings of matrix to generate</param>
/// <param name="cols"> - number of columns of matrix to generate</param>
/// <returns>gen_matrix_statuses</returns>
gen_matrix_statuses gen_matrix(float*** matrix, int strs, int cols);

/// <summary>
/// Multiples matrix1 to matrix2. Number of columns in the 1st matrix MUST be equal to number of strings in the 2nd one.
/// </summary>
/// <param name="matrix1"></param>
/// <param name="matrix2"></param>
/// <param name="result"> - the result of multiplying</param>
/// <param name="str1"> - number of strings in the 1st matrix</param>
/// <param name="col1"> - number of columns in the 1st matrix</param>
/// <param name="str2"> - number of strings in the 2nd matrix</param>
/// <param name="col2"> - number of columns in the 2st matrix</param>
/// <param name="result_str"> - number of strings in result matrix</param>
/// <param name="result_col"> - number of columns in result matrix</param>
/// <returns>mult_matrixes_statuses</returns>
mult_matrixes_statuses mult_matrixes(float** matrix1, float** matrix2, float*** result, int str1, int col1, int str2, int col2, int* result_str, int* result_col);

/// <summary>
/// Finds matrix determinate
/// </summary>
/// <param name="matrix"></param>
/// <param name="str"> - number of strings in the matrix</param>
/// <param name="col"> - number of columns in the matrix</param>
/// <param name="determinate"> - found determinate</param>
/// <returns>matrix_deter_statuses</returns>
matrix_deter_statuses find_matrix_deter(float** matrix, int str, int col, double* determinate);

void clear(int*** matrix, int strs);

#endif // HEADER_9