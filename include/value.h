/*
 * Describes potential values for clox, as well as the value array and 
 * corresponding actions with the value array
 *
 */
#ifndef clox_value_h
#define clox_value_h

#include "./common.h"

typedef double Value;

typedef struct {
	int capacity;
	int count;
	Value* values;
} ValueArray;

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);
void printValue(Value value);



#endif
