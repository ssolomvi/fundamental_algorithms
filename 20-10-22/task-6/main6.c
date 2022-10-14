#define _CRT_SECURE_NO_WARNINGS
#include "header6.h"
#define file_count 3


int main(void)
{
	char* substr = NULL, filepath1[FILENAME_MAX], filepath2[FILENAME_MAX], filepath3[FILENAME_MAX];
	int found_count = 0;
	read_substring(&substr);
	if (substr == NULL) {
		printf("Memory error happened\n");
		return 0;
	}

	found* found_substrs = NULL;
	printf("Enter %d paths to files where to search the substring:\n", file_count);
	if (scanf("%s %s %s", filepath1, filepath2, filepath3) != file_count) {
		printf("Error reading stdin\n");
		free(substr);
		return 0;
	}

	find_substr_statuses find_substr_s = find_substr("", &found_substrs, &found_count, file_count, filepath1, filepath2, filepath3);
	switch (find_substr_s)
	{
	case find_substr_malloc_error:
		printf("Memory allocation error happened\n");
		return -1;
	case find_substr_realloc_error:
		printf("Memory reallocation error happened\n");
		return -2;
	case find_substr_open_file_error:
		printf("Open file error\n");
		return -3;
	case find_substr_incorrect_substr:
		printf("Empty string passed to the function as an argument\n");
		return -4;
	default:
		print_found(found_substrs, found_count);
		break;
	}

	free(substr);
	free(found_substrs);

	return 0;
}
