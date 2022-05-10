/*
 * Describe debug functions, including disassembling chunks
 */

#include "../include/debug.h"
#include <stdio.h>

// analyze a chunk of code
void disassembleChunk(Chunk *chunk, const char *name) {
  printf("== %s ==\n", name);

  for (int offset = 0; offset < chunk->count;) {

    // we let disassemble instruction handle what the new offset is
    // since each instruction can have different sizes.
    offset = disassembleInstruction(chunk, offset);
  }
  printf("== done ==\n");
}

// all we need to do is print
static int simpleInstruction(const char *name, int offset) {
  printf("%s\n", name);
  return offset + 1;
}

// print and also give us the index of the constant in the function
static int constantInstruction(const char *name, Chunk *chunk, int offset) {
  uint8_t constant = chunk->code[offset + 1]; // get the value from the code
  printf("%-16s %4d '", name, constant);      // constant = offset within our \
															value array
  printValue(chunk->constants.values[constant]);
  printf("'\n");
  return offset + 2;
}

// disassemble the instruction to make debugging easier
int disassembleInstruction(Chunk *chunk, int offset) {
  printf("%04d ", offset);
  printf("%4d ", chunk->lines[offset]);

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

  case OP_NIL:
    return simpleInstruction("OP_NIL", offset);

  case OP_TRUE:
    return simpleInstruction("OP_TRUE", offset);

  case OP_FALSE:
    return simpleInstruction("OP_FALSE", offset);

  case OP_NOT:
    return simpleInstruction("OP_NOT", offset);

  case OP_EQUAL:
    return simpleInstruction("OP_EQUAL", offset);

  case OP_GREATER:
    return simpleInstruction("OP_GREATER", offset);

  case OP_LESS:
    return simpleInstruction("OP_LESS", offset);

  case OP_PRINT:
    return simpleInstruction("OP_PRINT", offset);

  case OP_POP:
    return simpleInstruction("OP_POP", offset);

  case OP_DEFINE_GLOBAL:
    return constantInstruction("OP_DEFINE_GLOBAL", chunk, offset);

  default:
    printf("Unknown opcode %d\n", instruction);
    return offset + 1;
  }
}
