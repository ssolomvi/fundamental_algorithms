#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>

double dichotomy(double function(double), double a, double b, double eps);
double function(double);

int main()
{
	double eps, a , b, res;
	int l;
	printf("Enter precision:\n");
	scanf("%d", &l);
	eps = pow(10, -l);
	printf("Enter range limits [a;b]:\n");
	scanf("%lf %lf", &a, &b);

	if (isnan(res = dichotomy(function, a, b, eps))) {
		printf("The entered range limits are incorrect, f(a) * f(b) MUST BE < 0 to use dichotomy method\n");
		return -1;
	}

	printf("The found root: %.*lf", l + 1, res);

	return 0;
}

double function(double x)
{
	return x * x - 1;
}

double dichotomy(double function(double), double a, double b, double eps)
{
	double f_a = function(a), f_b = function(b), funct_result_middle;
	if (f_a * f_b > 0) {
		return NAN;
	}

	while (fabs(a - b) > eps) {
		funct_result_middle = function((a + b) / 2);

		if (fabs(funct_result_middle) < eps) {
			break;
		}

		if (f_a * funct_result_middle > 0) {
			a = (a + b) / 2;
			f_a = funct_result_middle;
		}

		if (f_b * funct_result_middle > 0) {
			b = (a + b) / 2;
			f_b = funct_result_middle;
		}
	}

	return (a + b) / 2;
}
