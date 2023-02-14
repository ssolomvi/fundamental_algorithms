#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int op_prior(char* str)
{
    switch (*str)
    {
    case '~':
        return 3;

    case '?':
    case '!':
    case '+':
    case '&':
        return 2;

    case '|':
    case '-':
    case '<':
    case '=':
        return 1;
    }
    return 0;
}

bool op_left_assoc(char* c)
{
    switch (*c)
    {
    case '?':
    case '!':
    case '+':
    case '&':
    case '|':
    case '-':
    case '<':
    case '=':
        return true;
    }
    return false;
}

unsigned int op_arg_count(char* c)
{
    switch (*c)
    {
    case '?':
    case '!':
    case '+':
    case '&':
    case '|':
    case '-':
    case '<':
    case '=':
        return 2;

    default:
        return c - 'A';
    }
    return 0;
}