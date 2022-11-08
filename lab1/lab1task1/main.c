#include <stdio.h>
#include "header.h"
void help(char*);
void incorrect(char*);

int main (int argc, char** argv)
{
    if (argc <= 1) {
        incorrect(*argv);
        return 0;
    }

    if (!strcmp(argv[1], "--help")) {
        help(*argv);
        return 0;
    } 
    
    else if (argc == 2 || argc > 3) {
        incorrect(*argv);
        return 0;
    }
    
    unsigned long long whole_num = 0;
    if ((sscanf_s(argv[1], "%llu", &whole_num)) != 1) {
        printf("The number cannot be read\n");
    }

    if (!strcmp(argv[2], "-h") || !strcmp(argv[2], "/h")) {
        int* mults = NULL, size;
        mult_status mult_s = mult(whole_num, &mults, &size);
        switch (mult_s) 
        {
        case mult_no_multiples:
            printf("There are no multiples <= 100 for %llu\n", whole_num);
            break;

        case mult_malloc_error:
            printf("Memory allocation error happened\n");
            break;
        case mult_realloc_error:
            printf("Memory reallocation error happened\n");
            break;

        case mult_ok:
            printf("Found multiples for %llu:\n", whole_num);
            for (int i = 0; i < size; i++) {
                printf("%d ", mults[i]);
            }
            putchar('\n');
        default:
            break;
        }
    }


    else if (!strcmp(argv[2], "-p") || !strcmp(argv[2], "/p")) {
        is_prime_status is_prime_s = is_prime(whole_num);
        switch (is_prime_s)
        {
        case is_prime_is_1:
            printf("The entered number is 1, it's niether prime no composite\n");
            break;
        case is_prime_is_composite:
            printf("Number %llu is composite\n", whole_num);
            break;
        case is_prime_is_prime:
            printf("Number %llu is prime\n", whole_num);
            break;
        default:
            break;
        }
    }


    else if (!strcmp(argv[2], "-s") || !strcmp(argv[2], "/s")) {
        char* str = NULL;
        my_itoa_1_status my_itoa_1_s = my_itoa_1(whole_num, &str);
        switch (my_itoa_1_s)
        {
        case my_itoa_1_malloc_error:
            printf("Memory allocation error happened\n");
            break;
        case my_itoa_1_ok:
            printf("%s", str);
            break;
        default:
            break;
        }
    }


    else if (!strcmp(argv[2], "-e") || !strcmp(argv[2], "/e")) {
        in_pow_status in_pow_s = in_pow(whole_num);
        switch (in_pow_s)
        {
        case in_pow_incorrect_num:
            printf("Number %llu is > 10\n", whole_num);
            incorrect(*argv);
            break;
        default:
            break;
        }
    }

    else if (!strcmp(argv[2], "-a") || !strcmp(argv[2], "/a")) {
        unsigned long long summa;
        sum_status sum_s = sum(whole_num, &summa);
        switch (sum_s)
        {
        case sum_type_overflow:
            printf("Type overflow\n");
            break;
        case sum_ok:
            printf("The sum from 1 to %llu is %llu", whole_num, summa);
            break;
        default:
            break;
        }
    }


    else if (!strcmp(argv[2], "-f") || !strcmp(argv[2], "/f")) {
        unsigned long long factorial;
        fact_status fact_s = fact(whole_num, &factorial);
        switch (fact_s)
        {
        case fact_type_overflow:
            printf("Type overflow\n");
            break;
        case fact_ok:
            printf("The factorial of %llu is %llu", whole_num, factorial);
        default:
            break;
        }
    }
}


void help(char* argv)
{
    printf("The programm operates with whole positive numbers\n");
    printf("Syntax : \n\t%s [z] -* or /*\n", argv);
    printf("Add a flag:\n");
    printf("\t-h or /h -- prints multiples <= 100 to entered number\n");
    printf("\t-p or /p -- prints if entered number is prime or composite number\n");
    printf("\t-s or /s -- prints every digit of entered number through space\n");
    printf("\t-e or /e -- prints 1 ... 10 in pow 1 ... entered number if it is <= 10\n");
    printf("\t-a or /a -- sums from 1 to entered number\n");
    printf("\t-f of /f -- counts factorial of entered number\n");
}

void incorrect(char* argv)
{
    printf("Syntax : \n\t%s [z] -* or /*\n", argv);
    printf("For help type \n\t%s --help\n", argv);
}
