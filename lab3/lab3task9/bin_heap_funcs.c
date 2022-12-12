#include "bin_heap.h"

size_t parent(size_t i)
{
	return (i - 1) / 2;
}

size_t left_child(size_t i)
{
	return ((2 * i) + 1);
}

size_t right_child(size_t i)
{
	return ((2 * i) + 2);
}

void shiftUp(size_t i, bin_heap* heap)
{
    T* tmp = NULL;
    while (i > 0 && compare_prior(heap->heap[parent(i)], heap->heap[i]) < 0) {
        // Swap parent and current node
        tmp = heap->heap[parent(i)];
        heap->heap[parent(i)] = heap->heap[i];
        heap->heap[i] = tmp;

        // Update i to parent of i
        i = parent(i);
    }
}

void shiftDown(size_t i, bin_heap* heap)
{
    size_t maxIndex = i;
    T* tmp = NULL;

    // Left Child
    size_t left_ch = left_child(i);

    if (left_ch <= heap->size && compare_prior(heap->heap[left_ch], heap->heap[maxIndex]) > 0) {
        maxIndex = left_ch;
    }

    // Right Child
    size_t right_ch = right_child(i);

    if (right_ch <= heap->size && compare_prior(heap->heap[right_ch], heap->heap[maxIndex]) > 0) {
        maxIndex = right_ch;
    }

    // If i not same as maxIndex
    if (i != maxIndex) {
        tmp = heap->heap[i];
        heap->heap[i] = heap->heap[maxIndex];
        heap->heap[maxIndex] = tmp;
        shiftDown(maxIndex, heap);
    }
}

void delete_bin_heap(bin_heap* heap)
{
    size_t i;
    for (i = 0; i < heap->size; i++) {
        delete_string(&heap->heap[i]->text);
        free(heap->heap[i]);
    }
    free(heap->heap);
    heap->size = 0;
    heap->allocated = 0;
}

// Function to insert a new element
// in the Binary Heap
int insert_bin_heap(bin_heap* heap, T* to_insert)
{
    T** tmp = NULL;
    if (heap->allocated == 0) {
        heap->allocated = 8;
        if (!(heap->heap = (T**)malloc(sizeof(T*) * heap->allocated))) {
            return -1;
        }
    }
    
    if (heap->allocated == heap->size) {
        heap->allocated *= 2;
        if (!(tmp = (T**)realloc(heap->heap, sizeof(T*) * heap->allocated))) {
            delete_bin_heap(heap);
            return -1;
        }
        heap->heap = tmp;
    }

    heap->heap[heap->size] = to_insert;

    // Shift Up to maintain heap property
    shiftUp(heap->size, heap);
    heap->size++;

    return 0;
}

// Function to extract the element with
// maximum priority
T* extractMax(bin_heap* heap)
{
    T* result = heap->heap[0];

    // Replace the value at the root
    // with the last leaf
    heap->heap[0] = heap->heap[heap->size - 1];
    heap->size--;

    // Shift down the replaced element
    // to maintain the heap property
    shiftDown(0, heap);
    return result;

}

T* getMax(bin_heap heap)
{
    return heap.heap[0];
}

read_from_file_bin_heap_statuses read_from_file_bin_heap(bin_heap* heap, container* input_arguments, size_t count_of_files)
{
    FILE* curr_fi = NULL;
    size_t count_of_read_files = 0, curr_reading_number = 0;
    int create_T_response = 0;
    T* new_T = NULL;

    while (count_of_read_files != count_of_files) {
        if (input_arguments[curr_reading_number].is_read) {
            curr_reading_number++;
            if (curr_reading_number == count_of_files) {
                curr_reading_number = 0;
            }
            continue;
        }
        
        if (!(curr_fi = fopen(input_arguments[curr_reading_number].data.text.str, "r"))) {
            delete_bin_heap(heap);
            return read_from_file_bin_heap_open_file_error;
        }

        if (fseek(curr_fi, input_arguments[curr_reading_number].bytes_read, SEEK_SET) != 0) {
            delete_bin_heap(heap);
            return read_from_file_bin_heap_fseek_error;
        }

        if ((create_T_response = make_new_T(curr_fi, input_arguments[curr_reading_number].data.priority, &new_T)) == -1) {
            delete_bin_heap(heap); 
            return read_from_file_bin_heap_malloc_error;
        }

        if (create_T_response == -2) {
            input_arguments[curr_reading_number].is_read = 1;
            count_of_read_files++;
            curr_reading_number++;
            if (curr_reading_number == count_of_files) {
                curr_reading_number = 0;
            }
            continue;
        }

        if (insert_bin_heap(heap, new_T) != 0) {
            delete_string(&(new_T->text));
            delete_bin_heap(heap);
            return read_from_file_bin_heap_malloc_error;
        }

        input_arguments[curr_reading_number].bytes_read += (new_T->text.length + 2) * sizeof(char);
        fclose(curr_fi);

        curr_reading_number++;
        if (curr_reading_number == count_of_files) {
            curr_reading_number = 0;
        }
    }

    T** tmp = NULL;
    if (!(tmp = (T**)realloc(heap->heap, sizeof(T*) * heap->size))) {
        delete_bin_heap(heap);
        return read_from_file_bin_heap_realloc_error;
    }
    heap->allocated = heap->size;
    heap->heap = tmp;

    return read_from_file_bin_heap_ok;
}


void extract_n_print_bin_heap(FILE* stream, bin_heap* heap)
{
    if (!heap->size)
        return;

    T* extracted = NULL;

    while (heap->size) {
        extracted = extractMax(heap);
        print_T(stream, extracted);
        delete_string(&extracted->text);
        free(extracted);
    }
    
    free(heap->heap);
    heap->heap = NULL;
    heap->allocated = 0;
}

