#include <stdio.h>
#include <string.h>
#include "header.h"

void help();
void incorrect(char**);


int main(int argc, char** argv)
{
    if (argc <= 1) {
        incorrect(*argv);
        return 0;
    }


    if (!strcmp(argv[1], "--help")) {
        help();
        return 0;
    }

    if (argc < 4) {
        incorrect(*argv);
        return 0;
    }

    // give a solution to quadratic equation
    if (!strcmp(argv[1], "-q") || !strcmp(argv[1], "/q")) {
        long double a, b, c;

        if (sscanf_s(argv[2], "%Lf", &a) != 1 || sscanf_s(argv[3], "%Lf", &b) != 1 || sscanf_s(argv[4], "%Lf", &c) != 1 || argc > 5) {
            incorrect(argv);
            return 0;
        }

        else {
            double _1, _2;
            qes_statuses qes_s = quadratic_equation_solve(a, b, c, &_1, &_2);
            switch (qes_s)
            {
            case qes_not_qe:
                printf("The enetered coefficients do not form a quadratic equation\n");
                break;
            case qes_not_r:
                printf("Unfortunetly for you, the programm operates with REAL numbers\n");
                break;
            case qes_1_ans:
                printf("The answer is: %lf\n", _1);
                break;
            case qes_2_ans:
                printf("The answer is: %lf; %lf\n", _1, _2);
                break;
            case qes_overflow:
                printf("Error: type overflow\n");
            default:
                break;
            }
        }
    }

    // if the first num is multiple to the second num
    else if (!strcmp(argv[1], "-m") || !strcmp(argv[1], "/m")) {
        long long a, b;
        if (sscanf_s(argv[2], "%llu", &a) != 1 || sscanf_s(argv[3], "%llu", &b) != 1 || argc > 4) {
            incorrect(argv);
            return 0;
        }

        else {
            mult_statuses mult_s = is_multiple(a, b);
            switch (mult_s)
            {
            case mult_is_0:
                printf("The entered numbers should not be equal to 0\n");
                break;
            case mult_is_mult:
                printf("The number %llu is multiple to %llu", b, a);
                break;
            case mult_is_not_mult:
                printf("The number %llu is NOT multiple to %llu", b, a);
                break;
            default:
                break;
            }
        }
    }

    // if entered nums are sides of a right triangle
    else if (!strcmp(argv[1], "-t") || !strcmp(argv[1], "/t")) {
        long double a, b, c;
        if (sscanf_s(argv[2], "%Lf", &a) != 1 || sscanf_s(argv[3], "%Lf", &b) != 1 || sscanf_s(argv[4], "%Lf", &c) != 1 || argc > 5) {
            incorrect(argv);
            return 0;   
        }

        triangle_statuses triangle_s = is_right_triangle(a, b, c);
        switch (triangle_s)
        {
        case trian_is_r_trian:
            printf("The entered sides form right triangle\n");
            break;
        case trian_is_not_r_trian:
            printf("The entered sides do NOT form right triangle\n");
            break; 
        case trian_overflow:
            printf("Error: type overflow\n");
            break;
        default:
            break;
        }
    }

    else {
        incorrect(*argv);
    }

    return 0;

}


void help() 
{
    printf("The programm expects a flag and set of numbers\nAdd a flag:\n");
    printf("\t-q or /q -- Set of three numbers represents quadratic equation coefficients.\n");
    printf("\t            The programm will give a solution to equation.\n\n");
    printf("\t-m or /m -- Set of numbers consists of 2 nonzero integers.\n");
    printf("\t            The programm defines if the first one is multiple to the second one.\n\n");
    printf("\t-t or /t -- Set of numbers consists of nonzero numbers.\n");
    printf("\t            The programm defines if they are sides of a right triangle.\n");
}


void incorrect(char* argv)
{
    printf("Incorret syntax\n");
    printf("For help type \n\t%s --help\n", argv);
}
