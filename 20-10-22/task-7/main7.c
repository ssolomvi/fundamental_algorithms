#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <float.h>
#include <time.h>

typedef enum is_convex_statuses {
	is_convex_not_poligon,
	is_convex_incorrect_arguments,
	is_convex_is_convex_poligon,
} is_convex_statuses;

typedef enum find_polinom_statuses {
	find_polinom_num_error,
	find_polinom_overflow,
	find_polinom_pow_error,
	find_polinom_ok,
} find_polinom_statuses;

double gen_random_num();
is_convex_statuses is_convex(int count, ...);
find_polinom_statuses find_polinom_value(long double* res, double x, int power, ...);

int main(void)
{
	srand(time(NULL));
	
	int count;
	printf("Enter count of polinom apexes <= 7:\n");
	scanf("%d", &count);

	if (count > 7 || count < 3) {
		printf("Incorrect num of apexes\n");
		return -1;
	}

	double x1 = 2, y1 = 1, x2 = 5, y2 = 1, x3 = 8, y3 = 3, x4 = 7, y4 = 5, x5 = 5, y5 = 6, x6 = 3, y6 = 6, x7 = 1, y7 = 5;

	/*
	float x1 = gen_random_num(), x2 = gen_random_num(), x3 = gen_random_num(), x4 = gen_random_num();
	float x5 = gen_random_num(), x6 = gen_random_num(), x7 = gen_random_num(), y1 = gen_random_num();
	float y2 = gen_random_num(), y3 = gen_random_num(), y4 = gen_random_num(), y5 = gen_random_num();
	float y6 = gen_random_num(), y7 = gen_random_num();
	*/
	is_convex_statuses is_convex_s = is_convex(count, x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6, x7, y7);
	switch (is_convex_s)
	{
	case is_convex_not_poligon:
		printf("The entered coordinates do NOT form a convex poligon\n\n");
		break;
	case is_convex_incorrect_arguments:
		printf("Entered arguments of function is_convex are incorrect\n");
		return -1;
	default:
		printf("Wow, the entered coordinates DO form a convex poligon\n\n");
		break;
	}
	

	// double coef1 = gen_random_num(), coef2 = gen_random_num(), coef3 = gen_random_num(), coef4 = gen_random_num(), coef5 = gen_random_num(), coef6 = gen_random_num();
	double coef1 = -4, coef2 = 3, coef3 = 2, coef4 = 0, coef5 = -7, coef6 = 10;
	int power = 5;
	long double res;

	double x;
	printf("Enter coordinate x of a point for calculating the polinom's value and power of polinom <= 5:\n");
	scanf("%lf %d", &x, &power);
	if (power == 5) {
		printf("Pow error happened\n");
		return -3;
	}
	find_polinom_statuses find_polinom_s = find_polinom_value(&res, x, power, coef1, coef2, coef3, coef4, coef5, coef6);
	switch (find_polinom_s)
	{
	case find_polinom_num_error:
		printf("The entered x is incorrect\n");
		return -2;
	case find_polinom_pow_error:
		printf("Pow error happened\n");
		return -3;
	case find_polinom_overflow:
		printf("Type overflow happened\n");
		return -4;
	default:
		printf("The found value of polinom in x: %Lf\nCoefficients: %f %f %f %f %f %f\n", res, coef1, coef2, coef3, coef4, coef5, coef6);
		break;
	}

	return 0;
}

double gen_random_num()
{
	return (double)rand() / RAND_MAX + rand() % 100 - 50;
}

is_convex_statuses is_convex(int count, ...)
{
	if (count <= 2) {
		return is_convex_not_poligon;
	}

	if (count == 3) {
		return is_convex_is_convex_poligon;
	}

	short turn, direction = 0;
	va_list ptr;
	va_start(ptr, count);

	if (!ptr)
		return is_convex_incorrect_arguments;

	double first_vector_x = -va_arg(ptr, double);
	double first_vector_y = -va_arg(ptr, double);

	double prev_x = va_arg(ptr, double);
	double prev_y = va_arg(ptr, double);

	first_vector_x += prev_x;
	first_vector_y += prev_y;

	double ab_x = first_vector_x, ab_y = first_vector_y, bc_x, bc_y, curr_x, curr_y;
	int i = 1;

	for (i; i < count; i++)
	{
		curr_x = va_arg(ptr, double);
		curr_y = va_arg(ptr, double);

		bc_x = curr_x - prev_x;
		bc_y = curr_y - prev_y;
		
		turn = ab_x * bc_y - ab_y * bc_x;
		if ((turn > 0 && direction < 0) || (turn < 0) && direction > 0) {
			va_end(ptr);
			return is_convex_not_poligon;
		}
		if (!direction) {
			direction = turn;
		}
		ab_x = bc_x;
		ab_y = bc_y;
	}

	bc_x = first_vector_x;
	bc_y = first_vector_y;
	
	turn = ab_x * bc_y - ab_y * bc_x;
	if ((turn > 0 && direction < 0) || (turn < 0 && direction > 0)) {
		va_end(ptr);
		return is_convex_not_poligon;
	}

	va_end(ptr);
	return is_convex_is_convex_poligon;
}

find_polinom_statuses find_polinom_value(long double* res, double x, int power, ...)
{
	if (x == 0) {
		return find_polinom_num_error;
	}

	if (!power) {
		return find_polinom_pow_error;
	}

	(*res) = 0;
	long double to_add = 0, num_in_pow = pow(x, power);
	float curr_coef;
	va_list coef;
	va_start(coef, power);
	int i;

	for (i = 0; i <= power; i++) {
		curr_coef = va_arg(coef, double);
		to_add = num_in_pow * curr_coef;
		
		double t = LDBL_MAX - (*res);

		if (LDBL_MAX - (*res) > to_add)
			(*res) += to_add;
		else
			return find_polinom_overflow;
		if (i == power - 1) {

		}
		if (i == power) {
			num_in_pow = 1;
		}
		else {
			num_in_pow /= x;
		}
	}

	va_end(coef);
	return find_polinom_ok;
}
