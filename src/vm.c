#include "../include/common.h"
#include "../include/vm.h"
#include "../include/debug.h"
#include <stdio.h>

#define BINARY_OP(op) \
do { \
double b = pop(); \
double a = pop(); \
push(a op b); \
printf("\n"); \
} while (false)


VM vm;

// just set the top of the stack to index 0.
static void resetStack() {
	vm.stackTop = vm.stack;
}

void push(Value value) {
	*vm.stackTop = value;
	vm.stackTop++;
}

Value pop() {
	vm.stackTop--;
	return *vm.stackTop;
}

static InterpretResult run() {
	#define READ_BYTE() (*vm.ip++)
	#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

	for (;;) {

		// debug our VM
		#ifdef DEBUG_TRACE_EXECUTION

			// print our stack contents
			printf(" ");
			for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
				printf("[ ");
				printValue(*slot);
				printf(" ]");
			}
			printf("\n");

			// dissasemble instruction expects an integer offset into the
			// chunk in order to print it
			disassembleInstruction(vm.chunk,(int)(vm.ip - vm.chunk->code));
		#endif

		// decode the instruction
		uint8_t instruction;
		switch (instruction = READ_BYTE()) {
			case OP_CONSTANT: 
				Value constant = READ_CONSTANT();
				push(constant);
				printf("\n");
				break;
			
			case OP_NEGATE:
				push(-pop()); 
				printf("\n");
				break;
			
			case OP_RETURN: 
				printValue(pop());
				printf("\n");
				return INTERPRET_OK;

			case OP_DIVIDE:
				BINARY_OP(/);
				break;

			case OP_ADD:
				BINARY_OP(+);
				break;

			case OP_MULTIPLY:
				BINARY_OP(*);
				break;

			case OP_SUBTRACT:
				BINARY_OP(-);
				break;

			default:
				return INTERPRET_RUNTIME_ERROR;
	}
}

#undef READ_BYTE
#undef READ_CONSTANT
}



InterpretResult interpret(Chunk* chunk) {
	Chunk chunk;
	initChunk(&chunk);

	if (!compile(source, &chunk)) {
		freeChunk(&chunk);
		return INTERPRET_COMPILE_ERROR;
	}

	vm.chunk = &chunk;
	vm.ip = vm.chunk->code;

	InterpretResult result = run();

	freeChunk(&chunk);
	return result;





}


void initVM() {
	resetStack();
}

void freeVM() {
}
