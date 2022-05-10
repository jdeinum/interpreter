/*
 * Describe the structure of Chunks, as well as operations on them
 *
 */

#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

// types of operations
typedef enum {
  OP_RETURN,
  OP_CONSTANT,
  OP_NEGATE,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NIL,
  OP_TRUE,
  OP_FALSE,
  OP_NOT,
  OP_EQUAL,
  OP_GREATER,
  OP_NOT_EQUAL,
  OP_GREATER_EQUAL,
  OP_LESS_EQUAL,
  OP_LESS,
  OP_PRINT,
  OP_POP,
  OP_DEFINE_GLOBAL,
} OpCode;

// storage for instructions and data
typedef struct {
  int count;    // number of elements in the chunk
  int capacity; // number of available elements in this chunk
  uint8_t *code;
  ValueArray constants;
  int *lines;
} Chunk;

void initChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, uint8_t byte, int line);
void freeChunk(Chunk *chunk);
int addConstant(Chunk *chunk, Value value);

#endif
