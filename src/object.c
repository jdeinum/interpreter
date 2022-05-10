/*
 * Objects are:
 *		Strings
 *		Hash Tables
 *		...
 */




#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../include/memory.h"
#include "../include/object.h"
#include "../include/value.h"
#include "../include/vm.h"
#include "../include/table.h"

extern VM vm;

#define ALLOCATE_OBJ(type, objectType) \
(type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(size_t size, ObjType type) {
	Obj* object = (Obj*)reallocate(NULL, 0, size);
	object->type = type;

	object->next = vm.objects;
	vm.objects = object;
	return object;
}


static ObjString* allocateString(char* chars, int length, uint32_t hash) {
	ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
	string->length = length;
	string->chars = chars;
	string->hash = hash;
	tableSet(&vm.strings, string, NIL_VAL);
	return string;
}

// FNV-1a algorithm, short and sweet. We can replace this hash algorithm with
// our own if desired.
static uint32_t hashString(const char* key, int length) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < length; i++) {
    hash ^= (uint8_t)key[i];
    hash *= 16777619;
  }
  return hash;
}

ObjString* takeString(char* chars, int length) {
	uint32_t hash = hashString(chars, length);

	// check if the string already exists in our strings table
	// // if so, return a reference to it and free the newly created string
  ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
	if (interned != NULL) {
    FREE_ARRAY(char, chars, length + 1);
    return interned;
  }

  return allocateString(chars, length, hash);
}

ObjString* copyString(const char* chars, int length) {
	uint32_t hash = hashString(chars, length);

	// check if the string is already in our string table
	// if so, return a pointer to that one, otherwise, allocate it
	ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
  if (interned != NULL) return interned;

	char* heapChars = ALLOCATE(char, length + 1);
	memcpy(heapChars, chars, length);
	heapChars[length] = '\0';
	return allocateString(heapChars, length, hash);
}

void printObject(Value value) {
	switch (OBJ_TYPE(value)) {
	  case OBJ_STRING:
			printf("%s", AS_CSTRING(value));
			break;
	}
}


