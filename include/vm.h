#ifndef clox_vm_h
#define clox_vm_h


#include "chunk.h"
#include "table.h"
#include "object.h"


// stack related
#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)
void push(Value value);
Value pop();



typedef enum {
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR
} InterpretResult;


typedef struct {
  ObjFunction* function;
  uint8_t* ip;
  Value* slots;
} CallFrame;



InterpretResult interpret(const char* source);

typedef struct {
  CallFrame frames[FRAMES_MAX];
  int frameCount;
	Value stack[STACK_MAX];
	Value* stackTop;
	Obj* objects;
	Table strings;
	Table globals;
} VM; 

void initVM();
void freeVM();

#endif
