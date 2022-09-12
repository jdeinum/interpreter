/*
 * Define functions for interacting with chunks. Chunks are where our code and
 * constants are stored. The chunks are then passed to the VM to be executed.
 */

#include <stdio.h>

#include "../include/chunk.h"
#include "../include/memory.h"
#include "../include/value.h"


// initialize our chunk of bytecode
void initChunk(Chunk* chunk) {
	chunk->count = 0;
	chunk->capacity = 0;
	chunk->code = NULL;
	initValueArray(&chunk->constants);
	chunk->lines = NULL;
}

// write a byte to our chunk
void writeChunk(Chunk* chunk, uint8_t byte, int line) {

	// not enough room for the new byte, must allocate a new array
	if (chunk->capacity < chunk->count + 1) {
		int oldCapacity = chunk->capacity;
		chunk->capacity = GROW_CAPACITY(oldCapacity);
		chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity,
				chunk->capacity);
		chunk->lines = GROW_ARRAY(int, chunk->lines,
			oldCapacity, chunk->capacity);
	}


	
	// write the instruction to our chunk, as well as writing the line number
	// for the instruction into the chunk
	chunk->code[chunk->count] = byte;
	chunk->lines[chunk->count] = line;
	chunk->count++;
}


// free our chunk
void freeChunk(Chunk* chunk) {
	FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
	freeValueArray(&chunk->constants);
	FREE_ARRAY(int, chunk->lines, chunk->capacity);
	initChunk(chunk);
}

// add a constant to our value array
int addConstant(Chunk* chunk, Value value) {
	writeValueArray(&chunk->constants, value);
	return chunk->constants.count - 1; // return index so we can retrieve it l8
}
