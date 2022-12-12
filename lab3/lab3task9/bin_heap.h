#ifndef BIN_HEAP_9
#include "header9.h"

typedef struct bin_heap {
	size_t size;
	size_t allocated;
	T** heap;
} bin_heap;

typedef enum read_from_file_bin_heap_statuses {
	read_from_file_bin_heap_malloc_error,
	read_from_file_bin_heap_realloc_error,
	read_from_file_bin_heap_open_file_error,
	read_from_file_bin_heap_fseek_error,
	read_from_file_bin_heap_ok
} read_from_file_bin_heap_statuses;

read_from_file_bin_heap_statuses read_from_file_bin_heap(bin_heap* heap, container* input_arguments, size_t count_of_files);
void extract_n_print_bin_heap(FILE* stream, bin_heap* heap);

#endif // !BIN_HEAP_9
