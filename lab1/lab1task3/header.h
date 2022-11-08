#pragma once
#ifndef HEADER_H
#include <stdio.h>

typedef enum file_statuses {
    read_file_file_not_exists,
    read_file_allocation_error,
    write_file_create_error,
    open_files_ok,
    close_file_error,
    close_file_ok,
} file_statuses;

typedef enum incorrect_statuses {
    incorrect_syntax_error,
    incorrect_flag,
    incorrect_too_many_args,
} incorrect_statuses;

typedef enum replace_statuses{
    replace_allocation_error,
    replace_reallocation_error,
    replace_ok
} replace_statuses;


file_statuses open_files(const char* filePath_in, const char* filePath_out, FILE** input, FILE** ouput);
void delete_digits(FILE* file_r, FILE* file_w);
void count_letters(FILE* file_r, FILE* file_w);
void count_extra(FILE* file_r, FILE* file_w);
void replace_non_digits(FILE* file_r, FILE* file_w);
replace_statuses replace_special_way(FILE* file_r, FILE* file_w);

#endif // HEADER_H
