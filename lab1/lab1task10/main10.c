#define _CRT_SECURE_NO_WARNINGS
#include "header10.h"

int main()
{
	srand(time(NULL));
	//int str1 = rand() % 10 + 1, col1 = rand() % 10 + 1, str2 = col1, col2 = rand() % 10 + 1;
	//int str1 = rand() % 10 + 1, str2 = str1, col1 = str1, col2 = str1;
	int str1 = 2, str2 = str1, col1 = str1, col2 = str1;


	int str_res, col_res;
	float** matrix1, ** matrix2, ** mult_result;
	double deter_1 = 0, deter_2 = 0;

	gen_matrix_statuses gen_m_1 = gen_matrix(&matrix1, str1, col1), gen_m_2 = gen_matrix(&matrix2, str2, col2);
	if (gen_m_1 == gen_malloc_error || gen_m_2 == gen_malloc_error) {
		printf("Memory allocation error\n");
		return 0;
	}
	else {
		printf("Matrixes were generated successfully!\n\n");
		printf("The first matrix:\n");
		pr_matrix(matrix1, str1, col1);
		printf("\nThe second matrix:\n");
		pr_matrix(matrix2, str2, col2);
		putchar('\n');
	}

	mult_matrixes_statuses mult_m_s = mult_matrixes(matrix1, matrix2, &mult_result, str1, col1, str2, col2, &str_res, &col_res);
	switch (mult_m_s)
	{
	case mult_matrixes_malloc_error:
		printf("Memory allocation error\n");
		return 0;
	case mult_matrixes_incorrect_matrixes:
		printf("Oops! Number of columns in the 1st matrix is not equal to number of strings in the 2nd one\n");
		return 0;
	default:
		printf("The result of matrixes multypling:\n");
		pr_matrix(mult_result, str_res, col_res);
		putchar('\n');
		break;
	}

	matrix_deter_statuses matrix_d_s1 = find_matrix_deter(matrix1, str1, col1, &deter_1), matrix_d_s2 = find_matrix_deter(matrix2, str2, col2, &deter_2);
	if (matrix_d_s1 == matrix_deter_incorrect_matrix || matrix_d_s2 == matrix_deter_incorrect_matrix) {
		printf("Oops! Number of columns in one of the matrixes is not equal to its number of strings. Thus, matrix's determinate cannot be found\n");
	}
	else {
		printf("Found determinants of 1st matrix: %.6lf\t of the 2nd matrix: %.6lf\n", deter_1, deter_2);
	}


	clear(&matrix1, str1);
	clear(&matrix2, str2);
	clear(&mult_result, str_res);

	return 0;
}