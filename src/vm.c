#include "../include/common.h"
#include "../include/vm.h"
#include "../include/debug.h"
#include <stdio.h>
#include <stdarg.h>


// we use the do-while trick to ensure the statements end up in the same
// scope and reduce the probability of getting a compiler error due to an 
// extra semi-colon
#define BINARY_OP(valueType, op) \
do { \
if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
runtimeError("Operands must be numbers."); \
return INTERPRET_RUNTIME_ERROR; \
} \
double b = AS_NUMBER(pop()); \
double a = AS_NUMBER(pop()); \
push(valueType(a op b)); \
} while (false)


VM vm;

// just set the top of the stack to index 0.
static void resetStack() {
	vm.stackTop = vm.stack;
}

static void runtimeError(const char* format, ...) {
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fputs("\n", stderr);

	size_t instruction = vm.ip - vm.chunk->code - 1;
	int line = vm.chunk->lines[instruction];
	fprintf(stderr, "[line %d] in script\n", line);
	resetStack();
}

void push(Value value) {
	*vm.stackTop = value;
	vm.stackTop++;
}

Value pop() {
	vm.stackTop--;
	return *vm.stackTop;
}

static Value peek(int distance) {
	return vm.stackTop[-1 - distance];
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
				if (!IS_NUMBER(peek(0))) {
					runtimeError("Operand must be a number.");
					return INTERPRET_RUNTIME_ERROR;
				}

				push(NUMBER_VAL(-AS_NUMBER(pop())));
				break;


			
			case OP_RETURN: 
				printValue(pop());
				printf("\n");
				return INTERPRET_OK;

			case OP_ADD:      BINARY_OP(NUMBER_VAL, +); break;
			case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -); break;
			case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *); break;
			case OP_DIVIDE:   BINARY_OP(NUMBER_VAL, /); break;



			default:
				return INTERPRET_RUNTIME_ERROR;
	}
}

#undef READ_BYTE
#undef READ_CONSTANT
}




InterpretResult interpret(const char* source) {
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