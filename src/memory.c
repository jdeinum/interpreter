#include "../include/memory.h"
#include "../include/object.h"
#include "../include/vm.h"
#include <stdlib.h>

extern VM vm;

// handles allocating memory, freeing memory, and growing/shrinking memory
void *reallocate(void *pointer, size_t oldSize, size_t newSize) {
  if (newSize == 0) {
    free(pointer);
    return NULL;
  }

  // if pointer is NULL, realloc acts like malloc
  void *result = realloc(pointer, newSize);
  if (result == NULL)
    exit(1);
  return result;
}

static void freeObject(Obj *object) {
  switch (object->type) {
  case OBJ_STRING: {
    ObjString *string = (ObjString *)object;
    FREE_ARRAY(char, string->chars, string->length + 1);
    FREE(ObjString, object);
    break;
  }
  }
}

void freeObjects() {
  Obj *object = vm.objects;
  while (object != NULL) {
    Obj *next = object->next;
    freeObject(object);
    object = next;
  }
}
