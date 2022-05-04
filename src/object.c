#include <stdio.h>
#include <string.h>

#include "../include/memory.h"
#include "../include/object.h"
#include "../include/value.h"
#include "../include/vm.h"

ObjString* copyString(const char* chars, int length) {
	char* heapChars = ALLOCATE(char, length + 1);
	memcpy(heapChars, chars, length);
	heapChars[length] = '\0';
	return allocateString(heapChars, length);
}
