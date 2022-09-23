#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "task3-header.h"
#define digits(x) ((x) >= '0' && (x) <= '9')
#define letters(x) ((x) >= 'A' && (x) <= 'Z' || (x) >= 'a' && (x) <= 'z')

file_statuses open_files(const char* filePath_in, const char* filePath_out, FILE** input, FILE** output)
{
    if (!((*input) = fopen(filePath_in, "r"))) {
        return read_file_file_not_exists;
    }

    if (filePath_out) {
        if (!((*output) = fopen(filePath_out, "a"))) {
            return write_file_create_error;
        }
    }
    else
    {
        char* name;
        char* fpi = filePath_in, *lastBackslashPtr = NULL;

        while (*fpi)
        {
            if (*fpi == '\\')
            {
                lastBackslashPtr = fpi;
            }

            fpi++;
        }

        if (!(name = malloc(sizeof(char) * (strlen(filePath_in) + 5))))
        {
            return read_file_allocation_error;
        }

        if (lastBackslashPtr == NULL)
        {
            // relative path
            
            strcpy(name, "out_");
            strcpy(name + 4, filePath_in);
        }
        else
        {
            // absolute path
            
            int firstPartLength;
            strncpy(name, filePath_in, firstPartLength = lastBackslashPtr - filePath_in + 1);
            strcpy(name + firstPartLength, "out_");
            strcpy(name + firstPartLength + 4 /*strlen("out_")*/, lastBackslashPtr + 1);
            int x = 15;
        }

        *output = fopen(name, "a");
        free(name);

        if (!(*output)) {
            return write_file_create_error;
        }
    }
    return open_files_ok;
}

file_statuses close_files(FILE* input, FILE* output)
{
    // TODO: ?!
    if (fclose(input) == NULL || fclose(output) == NULL) {
        return close_file_error;
    }
    return close_file_ok;
}

void delete_digits(FILE* file_r, FILE* file_w)
{
    char c;
    while ((c = fgetc(file_r)) != EOF) {
        if (!digits(c)) {
            fputc(c, file_w);
        }
    }
}

void reverse(char* s)
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

const char* my_itoa(long number)
{
    char* s = (char*)malloc(sizeof(char)), *tmp;
    int iterator = 0, sign = 0;

    if (number < 0) {
        sign--; number = -number;
    }
    do {
        if (!(tmp = (char*)realloc(s, (iterator + 2) * sizeof(char)))) {
            free(s);
            return NULL;
        }
        else {
            s = tmp;
            s[iterator++] = number % 10 + '0';
        }
    } while ((number /= 10) > 0);

    if (sign < 0) s[iterator++] = '-';
    s[iterator] = '\0';
    reverse(s);
    return s;
}

void count_letters(FILE* file_r, FILE* file_w)
{
    long count_let = 0;
    char c;
    char* allocated;
    while ((c = fgetc(file_r)) != EOF) {
        if (c == '\n') {
            fputs(allocated = my_itoa(count_let), file_w);
            free(allocated);
            fputc('\n', file_w);
            count_let = 0;
        }
        if (letters(c)) {
            count_let++;
        }
    }
}


void count_extra(FILE* file_r, FILE* file_w)
{
    long count_ex = 0;
    char c;
    char* allocated;
    while ((c = fgetc(file_r)) != EOF) {
        if (c == '\n') {
            fputs(allocated = my_itoa(++count_ex), file_w);
            free(allocated);
            fputc('\n', file_w);
            count_ex = 0;
        }
        else if (!digits(c) && !letters(c) && c != ' ') {
            count_ex++;
        }
    }
}

void replace_non_digits(FILE* file_r, FILE* file_w)
{
    char c;
    int num;
    char* allocated;

    while ((c = fgetc(file_r)) != EOF) {
        if (!digits(c)) {
            num = (int)c;
            fputs(allocated = my_itoa(num), file_w);
            free(allocated);
        }
    }
}

char to_lower(char c)
{
    return (c >= 'A' && c <= 'Z') ? c + ' ' : c;
}


char* replace_ascii(char* str, int size) {
    char* new_str = NULL, *ascii_code_str;

    if (!(new_str = (char*)malloc((size * 3 + 1) * sizeof(char)))) {
        return NULL;
    }

    int i, j, ascii_code_num, new_str_size = 0;
    
    for (i = 0; i < size; i++) {
        ascii_code_num = (int)str[i];
        
        strcpy(new_str + new_str_size, ascii_code_str = my_itoa(ascii_code_num));
        new_str_size += strlen(ascii_code_str);
        free(ascii_code_str);
    }

    return new_str;
}

replace_statuses replace_special_way(FILE* file_r, FILE* file_w)
{
    char* s = NULL, *tmp = NULL;
    char c, prev = 0;
    int size = 0;
    unsigned long counter = 0, lex_size = 64;
    while (!feof(file_r))
    {
        c = fgetc(file_r);

        if (!isspace(c) && isspace(prev) || (!isspace(c) && prev == 0)) {
            // lexeme starts
            s = (char*)malloc(sizeof(char) * lex_size);
            if (!s) {
                return replace_allocation_error;
            }
            else {
                s[size++] = c;
            }
        }
        else if (!isspace(c) && !isspace(prev)) {
            // lexeme continuos
            if (size == lex_size - 1) {
                lex_size *= 2;
                if (!(tmp = (char*)realloc(s, lex_size * sizeof(char)))) {
                    free(s);
                    free(tmp);
                    return replace_reallocation_error;
                }
                s = tmp;
            }
            s[size++] = c;
        }
        else if (isspace(c) && !isspace(prev)) {
            // lexeme ends
            if (prev != 0) {
                counter++;

                s[size] = '\0';
                if (!(tmp = (char*)realloc(s, (size + 1) * sizeof(char)))) {
                    free(s);
                    return replace_reallocation_error;
                }

                if (!(counter % 2)) {
                    int i = 0;
                    
                    int strlen_s = strlen(s);
                    while (strlen_s != 0 && i < strlen_s) {
                        s[i] = to_lower(s[i]);
                        i++;
                    }
                    s[i] = '\0';
                }
                if (!(counter % 5)) {
                    if (!(s = replace_ascii(s, size))) {
                        return replace_allocation_error;
                    }
                }
                size = 0;
                fputs(s, file_w);
                free(s);
            }
        }
        if (isspace(c))
            fputc(c, file_w);
        prev = c;
    }
    return replace_ok;
}
