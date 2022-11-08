#pragma once
#ifndef HEADER_1

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>

typedef enum mult_status {
	mult_no_multiples,
	mult_malloc_error,
	mult_realloc_error,
	mult_ok,
} mult_status;

typedef enum is_prime_status {
	is_prime_is_composite,
	is_prime_is_1,
	is_prime_is_prime,
} is_prime_status;

typedef enum my_itoa_1_status {
	my_itoa_1_malloc_error,
	my_itoa_1_ok,
} my_itoa_1_status;

typedef enum in_pow_status {
	in_pow_incorrect_num,
	in_pow_ok
} in_pow_status;

typedef enum sum_status {
	sum_type_overflow,
	sum_ok,
} sum_status;

typedef enum fact_status {
	fact_type_overflow,
	fact_ok
} fact_status;


// prints multiples to argv[1] <= 100
mult_status mult(unsigned long long number, int** mults, int* size);
// prints if argv[1] is prime or composite number
is_prime_status is_prime(unsigned long long number);
// prints every digit of argv[1] through space
my_itoa_1_status my_itoa_1(unsigned long long number, char** str);
// prints 1 ... 10 in pow 1 ... argv[1] if argv[1] is <= 10
in_pow_status in_pow(unsigned long long number);
// sums from 1 to argv[1]
sum_status sum(unsigned long long number, unsigned long long* sum);
// fact(argv[1])
fact_status fact(unsigned long long number, unsigned long long* result);

#endif // !HEADER_1
