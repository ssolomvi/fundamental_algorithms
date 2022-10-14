#define _CRT_SECURE_NO_WARNINGS
#include "header3.h"


int main(void)
{
	int l;
	printf("Enter precision:\n");
	scanf("%d", &l);
 	double eps = pow(10, -l);
	l++; // as using %.*lf rounds the number (l = 2: 0.578 -> 0.58)
	
	printf("e constant:\n");
	printf("\tas limit value: %.*Lf\n", l, limit(e_lim, eps));
	printf("\tas row sum: %.*Lf\n", l, sum(e_row, eps, 0));
	printf("\tas solution of an equation: %.*lf\n\n", l, equation_dichotomy(e_equation, 2., 3., eps));

	printf("pi constant:\n");
	printf("\tas limit value: %.*Lf\n", l, limit(pi_lim, eps));
	printf("\tas row sum: %.*Lf\n", l, sum(pi_row, eps, 1));
	printf("\tas solution of an equation: %.*lf\n\n", l, equation_dichotomy(pi_equation, 3, 3.5, eps));
	
	printf("ln(2) constant:\n");
	printf("\tas limit value: %.*lf\n", l, limit(ln2_lim, eps));
	printf("\tas row sum: %.*lf\n", l, sum(ln2_row, eps, 1));
	printf("\tas solution of an equation: %.*lf\n\n", l, equation_dichotomy(ln2_equation, 0, 1, eps));

	printf("sqrt(2) constant:\n");
	printf("\tas limit value: %.*lf\n", l, sqrt2_lim(eps));
	printf("\tas row product: %.*lf\n", l, mult(sqrt2_mult, eps, 2));
	printf("\tas solution of an equation: %.*lf\n\n", l, equation_dichotomy(sqrt2_equation, 1, 2, eps));
	
	printf("Euler's constant:\n");
	printf("\tas limit value: %.*lf\n", l, euler_lim(eps));
	double constant_pi = M_PI * M_PI / 6;
	printf("\tas row sum: %.*lf\n", l, gamma_row(eps, 2) - constant_pi);
	printf("\tas solution of an equation: %.*lf\n\n", l, euler_equation(eps));

	return 0;
}
