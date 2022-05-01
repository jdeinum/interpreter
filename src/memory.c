#include <stdlib.h>
#include "../include/memory.h"

// handles allocating memory, freeing memory, and growing/shrinking memory
void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
	if (newSize == 0) {
		free(pointer);
		return NULL;
	}

	void* result = realloc(pointer, newSize);
	if (result == NULL) exit(1);
	return result;
}




