#pragma once
#ifndef HEADER_3
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/// <summary>
/// Finds an aproximate number as a limit of an expression with given epsilon
/// </summary>
/// <param name="lim">- expression</param>
/// <param name="eps">- given epsilon</param>
/// <returns>a found limit / long double</returns>
long double limit(long double lim(int n), double eps);
long double e_lim(int n);
long double pi_lim(int n);
long double ln2_lim(int n);
long double sqrt2_lim(double eps);
long double euler_lim(double eps);

/// <summary>
/// Finds an aproximate number as a sum of "function" returned values with given epsilon
/// </summary>
/// <param name="function">- function which returns values</param>
/// <param name="eps">- given epsilon</param>
/// <param name="start">- value to start summing from</param>
/// <returns>found aproximate number</returns>
long double sum(long double function(int n), double eps, int start);

/// <summary>
/// Finds an aproximate number as a multiple of "function" returned values with given epsilon
/// </summary>
/// <param name="function">- function which returns values</param>
/// <param name="eps">- given epsilon</param>
/// <param name="start">- value to start multipling from</param>
/// <returns>found aproximate number</returns>
long double mult(long double function(int n), double eps, int start);
long double e_row(int n);
long double pi_row(int n);
long double ln2_row(int n);
long double sqrt2_mult(int k);
long double gamma_row(double eps, int start);

double equation_dichotomy(double equation(double x), double a, double b, double eps);
double e_equation(double x);
double pi_equation(double x);
double ln2_equation(double x);
double sqrt2_equation(double x);
double euler_equation(double eps);

#endif // HEADER_3
