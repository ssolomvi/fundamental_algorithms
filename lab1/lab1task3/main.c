#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

void help();
void incorrect(char**);

int main(int argc, char** argv)
{
    if (argc <= 1) {
        incorrect(argv);
        return 0;
    }


    if (!strcmp(argv[1], "--help")) {
        help();
        return 0;
    }

    char* s = argv[1];
    char to_do;
    short flag = 0;

    if (strlen(s) > 3 || (s[0] != '-' && s[0] != '/')) {
        incorrect(argv);
        return 0;
    }
    else if (strlen(s) == 3) {
        if (s[1] == 'n') {
            to_do = s[2];
        }
        else if (s[2] == 'n') {
            to_do = s[1];
        }
        else {
            incorrect(argv);
            return 0;
        }
        flag++;
    }
    else {
        to_do = s[1];
    }

    if (flag == 1 && argc != 4) {
        incorrect(*argv);
    }

    FILE* input = NULL, *output = NULL;
    open_files(argv[2], flag == 1 ? argv[3] : NULL, &input, &output);

    if (to_do == 'd') {
        delete_digits(input, output);
    }

    if (to_do == 'i') {
        count_letters(input, output);
    }

    if (to_do == 's') {
        count_extra(input, output);
    }
    
    if (to_do == 'a') {
        replace_non_digits(input, output);
    }
    
    if (to_do == 'f') {
        replace_statuses replace_s = replace_special_way(input, output);
        switch (replace_s)
        {
        case replace_allocation_error:
            printf("Allocation error happened\n");
            break;
        case replace_reallocation_error:
            printf("Reallocation error happened\n");
            break;
        default:
            break;
        }
    }

    fclose(input);
    fclose(output);
    return 0;
}

void help()
{
    printf("The programm expects a flag and the path  to the file\n");
    printf("If flag contains the character \"n\" the result of programm is saved\nin file which name is passed as the third argument\n\n");
    printf("If \"n\" is missing, the result of programm is saved as out_[entered_file_name]\n\n");
    printf("Add a flag:\n");
    printf("\t-d or /d -- delete digits\n"); 
    printf("\t-i or /i -- counts entries of letters in every string\n");
    printf("\t-s or /s -- counts entries of symbols unlike digits, letters and space\n");
    printf("\t-a or /a -- replaces symboles unlike digits with their ASCII code\n");
    printf("\t-f or /f -- makes every second lexeme's letters lowercase and replaces\n");
    printf("\t            every fifth lexeme's symboles with their ASCII code\n");
}

void incorrect(char** argv)
{
    printf("Incorret syntax\n");
    printf("For help type \n\t%s --help\n", argv[0]);
}
