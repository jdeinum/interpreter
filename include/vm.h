#ifndef clox_vm_h
#define clox_vm_h


#include "chunk.h"


// stack related
#define STACK_MAX 256
void push(Value value);
Value pop();



typedef enum {
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR
} InterpretResult;



InterpretResult interpret(const char* source);

typedef struct {
	Chunk* chunk;
	// keep track of which instruction we're at
	// using a pointer is faster than using an index
	uint8_t* ip; 
	Value stack[STACK_MAX];
	Value* stackTop;
	Obj* objects;
} VM; 

void initVM();
void freeVM();

#endif
