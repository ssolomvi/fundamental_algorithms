#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <float.h>
#include <time.h>

double geometric_mean(int count, ...);
void my_pow(double* res, double number, int _pow);
double gen_number(int min, int max);

int main(void)
{
	srand(time(NULL));
	// let the count be 3
	int count = 3, pow;
	double num1 = gen_number(1, 10), num2 = gen_number(1, 10), num3 = gen_number(1, 10), num4, res_pow = 1;
	double geom_mean = geometric_mean(count, num1, num2, num3);
	printf("The geometric mean of %d numbers [%lf %lf %lf]: %lf\n\n", count, num1, num2, num3, geom_mean);

	printf("Enter number and it's pow to calculate:\n");
	scanf("%lf %d", &num4, &pow);
	my_pow(&res_pow, num4, pow);
	printf("Number %lf in pow %d: %.15lf\n", num4, pow, res_pow);

	return 0;
}

double gen_number(int min, int max)
{
	double num = (double)rand() / RAND_MAX + rand() % max + min;
	return num;
}

double geometric_mean(int count, ...) {
	int i = 0;
	double res = 1.0, curr;
	va_list ptr;
	va_start(ptr, count);

	for (i; i < count; i++) {
		curr = va_arg(ptr, double);
		if (DBL_MAX / res < curr) {
			return NAN;
		}
		res *= curr;
	}

	if (res < 0) {
		return NAN;
	}

	va_end(ptr);
	return pow(res, 1.0 / count);
}

void my_pow(double* res, double number, int _pow)
{
	if (_pow != 0) {
		if (_pow < 0) {
			(*res) /= number;
			my_pow(res, number, ++_pow);
		}
		else {
			(*res) *= number;
			my_pow(res, number, --_pow);
		}
	}
	return;
}
