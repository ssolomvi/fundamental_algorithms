#pragma once
#ifndef HEADER_2
#include <float.h>
#include <math.h>
#define EPS 1e-8

typedef enum qes_statuses {
	qes_not_qe,
	qes_not_r,
	qes_1_ans,
	qes_2_ans,
	qes_overflow
} qes_statuses;

typedef enum mult_statuses {
	mult_is_mult,
	mult_is_not_mult,
	mult_is_0
} mult_statuses;


typedef enum triangle_statuses {
	trian_is_r_trian,
	trian_is_not_r_trian,
	trian_overflow
} triangle_statuses;

/// <summary>
/// Function solves quadratic equation: a*x^2 + b*x + c
/// </summary>
/// <param name="a">-- coefficient of x^2</param>
/// <param name="b">-- coefficient of x</param>
/// <param name="c">-- free coefficient</param>
/// <param name="_1">-- the first answer</param>
/// <param name="_2">-- the second answer</param>
/// <returns>Number of answers in R</returns>
qes_statuses quadratic_equation_solve(long double a, long double b, long double c, double* _1, double* _2);

/// <summary>
/// Function answers if the first parametre multiple to the second one
/// </summary>
/// <param name="_1">The first parametre</param>
/// <param name="_2">The second parametre</param>
/// <returns>An answer if the 1st value multiple to the 2nd</returns>
mult_statuses is_multiple(long long _1, long long  _2);

/// <summary>
/// Function answers if the entered values form the right triangle
/// </summary>
/// <param name="side_1">The 1st side</param>
/// <param name="side_2">The 2nd side</param>
/// <param name="side_3">The 3rd side</param>
/// <returns>An answer if the entered values form the right triangle</returns>
triangle_statuses is_right_triangle(long double side_1, long double side_2, long double side_3);

#endif // !HEADER_2
