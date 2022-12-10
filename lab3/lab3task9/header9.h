#define _CRT_SECURE_NO_WARNINGS

#ifndef HEADER_9
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string6.h"

typedef struct data {
	string text;
	int priority;
} T;

typedef struct doubly_linked_list {
	T* data;
	struct doubly_linked_list* next;
	struct doubly_linked_list* prev;
} doubl_list;

typedef struct binary_heap {
	int a;
} bin_heap;

# endif // HEADER_9
