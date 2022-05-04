/*
 * Define all of our memory management functions
 */



#ifndef clox_memory_h
#define clox_memory_h

#include "common.h"


// --------------------------------------------------------------------------
// All of these macros use reallocate to perform their functions




#define GROW_CAPACITY(capacity) \
((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, oldCount, newCount) \
(type*)reallocate(pointer, sizeof(type) * (oldCount), \
sizeof(type) * (newCount))

#define FREE_ARRAY(type, pointer, oldCount) \
reallocate(pointer, sizeof(type) * (oldCount), 0)


#define ALLOCATE(type, count) \
(type*)reallocate(NULL, 0, sizeof(type) * (count))

void* reallocate(void* pointer, size_t oldSize, size_t newSize);

#endif


