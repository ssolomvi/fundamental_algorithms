#include <stdio.h>
#include <stdlib.h>
#include "header1.h"


mult_status mult(unsigned long long number, int** mults, int* size)
{
        if (number > 100) {
            return mult_no_multiples;
        }


        int i;
        if (!(*mults = (int*)malloc(100 / number * sizeof(int)))) {
            free(*mults);
            return mult_malloc_error;
        }

        for (i = 1, *size = 0; i <= 100 / number; i++, (* size)++) {
            (*mults)[*size] = number * i;
        }
        
        return mult_ok;
}

is_prime_status is_prime(unsigned long long number)
{
    if (number == 1) {
        return is_prime_is_1;
    }
    if (number % 2 == 0 || number % 3 == 0) {
        return is_prime_is_composite;
    }
    
    int i = 3;
    while (i < floor(sqrt(number))) {
        i += 2;
        if (!(number % i)) {
            return is_prime_is_composite;
        }
        i += 4;
        if (!(number % i)) {
            return is_prime_is_composite;
        }
    }
    return is_prime_is_prime;
}


my_itoa_1_status my_itoa_1(unsigned long long number, char ** str)
{
    unsigned long long num2 = number;
    int size, size2;

    for (size = 0; num2 != 0; size++) {
        num2 /= 10;
    }
    size *= 2;

    if (!(*str = (char*)malloc((size + 1) * 2 * sizeof(char)))) {
        return my_itoa_1_malloc_error;
    }

    (*str)[size] = '\0';

    while (number != 0) {
        (*str)[size - 1] = number % 10 + '0';
        size--;
        (*str)[size - 1] = ' ';
        size--;
        number /= 10;
    }
    return my_itoa_1_ok;
}

in_pow_status in_pow(unsigned long long number)
{
    if (number > 10) {
        return in_pow_incorrect_num;
    }

    int i, j;
    long long k;
    for (i = 1; i < 11; i++) {
        k = i;
        for (j = number; j > 0; j--) {
            printf("%lld ", k);
            k *= i;
        }
        putchar('\n');
    }
    return in_pow_ok;
}

sum_status sum(unsigned long long number, unsigned long long* sum)
{
    if (ULLONG_MAX / (number + 1) >= number) {
        *sum = number * (number + 1) / 2;
        return sum_ok;
    }
    return sum_type_overflow;
}


fact_status fact(unsigned long long number, unsigned long long *factorial)
{
    *factorial = 1;
    while (number > 0) {
        if (ULLONG_MAX / *factorial >= number) {
            *factorial *= number;
            number--;
        }

        else {
            return fact_type_overflow;
        }
    }
    return fact_ok;
}
