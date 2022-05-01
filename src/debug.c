/*
 * Describe debug functions, including disassembling chunks
 */


#include <stdio.h>
#include "../include/debug.h"

void disassembleChunk(Chunk* chunk, const char* name) {
	printf("== %s ==\n", name);

	for (int offset = 0; offset < chunk->count;) {
		
		// we let disassemble instruction handle what the new offset is
		// since each instruction can have different sizes.
		offset = disassembleInstruction(chunk, offset);
	}
}


// all we need to do is print
static int simpleInstruction(const char* name, int offset) {
	printf("%s\n", name);
	return offset + 1;

}

static int constantInstruction(const char* name, Chunk* chunk,
                               int offset) {
	uint8_t constant = chunk->code[offset + 1];
	printf("%-16s %4d '", name, constant);
	printValue(chunk->constants.values[constant]);
	printf("'\n");
	return offset + 2;
}


// disassemble the instruction to make debugging easier
int disassembleInstruction(Chunk* chunk, int offset) {
	printf("%04d ", offset);
	
	// same line number as previously, no need to print line number for clarity
	if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
		printf(" | ");
	} 
	
	// new line number
	else {
		printf("%4d ", chunk->lines[offset]);
	}


	// print correct instruction type and increment offset in the correct
	// fashion
	uint8_t instruction = chunk->code[offset];
	switch (instruction) {
		case OP_CONSTANT:
			return constantInstruction("OP_CONSTANT", chunk, offset);

		case OP_RETURN:
			return simpleInstruction("OP_RETURN", offset);

		case OP_NEGATE:
			return simpleInstruction("OP_NEGATE", offset);

		case OP_ADD:
			return simpleInstruction("OP_ADD", offset);

		case OP_MULTIPLY:
			return simpleInstruction("OP_MULTIPLY", offset);

		case OP_SUBTRACT:
			return simpleInstruction("OP_SUBTRACT", offset);

		case OP_DIVIDE:
			return simpleInstruction("OP_DIVIDE", offset);
		default:
			printf("Unknown opcode %d\n", instruction);
			return offset + 1;
	}
}



