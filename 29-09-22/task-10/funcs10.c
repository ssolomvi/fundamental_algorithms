#include "header10.h"

void pr_matrix(float** matrix, int strs, int cols)
{
	int i, j;
	for (i = 0; i < strs; i++) {
		for (j = 0; j < cols; j++) {
			printf("%.2f ", matrix[i][j]);
		}
		putchar('\n');
	}
}

gen_matrix_statuses gen_matrix(float*** matrix, int strs, int cols)
{
	if (!((*matrix) = (float**)malloc(sizeof(float*) * strs))) {
		return gen_malloc_error;
	}

	// srand(time(NULL));
	int i, j, number;

	for (i = 0; i < strs; i++) {
		if (!((*matrix)[i] = (float*)malloc(sizeof(float) * cols))) {
			free((*matrix));
			return gen_malloc_error;
		}

		for (j = 0; j < cols; j++) {
			number = rand() % 201 - 100;
			(*matrix)[i][j] = number;
		}
	}
	return gen_ok;
}

mult_matrixes_statuses mult_matrixes(float** matrix1, float** matrix2, float*** result, int str1, int col1, int str2, int col2, int* result_str, int* result_col)
{
	if (!((*result) = (float**)malloc(sizeof(float*) * str1))) {
		return mult_matrixes_malloc_error;
	}

	int i, j, n;
	if (col1 != str2) {
		return mult_matrixes_incorrect_matrixes;
	}

	(*result_str) = str1; (*result_col) = col2;

	for (i = 0; i < str1; i++) {
		if (!((*result)[i] = (float*)malloc(sizeof(float) * (*result_col)))) {
			return gen_malloc_error;
		}

		for (j = 0; j < col2; j++) {
			(*result)[i][j] = 0;

			for (n = 0; n < col1; n++) {
				(*result)[i][j] += matrix1[i][n] * matrix2[n][j];
			}
		}
	}

	return mult_matrixes_ok;
}

matrix_deter_statuses find_matrix_deter(float** matrix, int str, int col, double * determinant)
{
	if (str != col) {
		return matrix_deter_incorrect_matrix;
	}

	if (str == 1) {
		(*determinant) = matrix[0][0];
		return matrix_deter_ok;
	}

	(*determinant) = 1;

	int i, j, k;
	double mult, make_null;
	for (j = 0; j < col - 1; j++) {
		for (i = 1 + j; i < str; i++) {
			if (matrix[i][j] != 0) {
				mult = (double)abs(matrix[i][j]) / abs(matrix[j][j]);
				for (k = col - 1; k >= 0; k--) {
					make_null = matrix[j][k] * mult;
					if (matrix[i][j] * matrix[j][j] >= 0) {
						matrix[i][k] -= make_null;
					}
					else {
						matrix[i][k] += make_null;
					}
				}
				putchar('\n');
				pr_matrix(matrix, str, col);
				putchar('\n');

			}
		}
		(*determinant) *= matrix[j][j];
	}
	
	(*determinant) *= matrix[j][j];
	return matrix_deter_ok;
}
// 1856 -- 64554
void clear(float*** matrix, int strs)
{
	int i;
	for (i = 0; i < strs; i++) {
		free((*matrix)[i]);
	}
	free((*matrix));
}