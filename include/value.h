/*
 * Describes potential values for clox, as well as the value array and 
 * corresponding actions with the value array
 *
 */
#ifndef clox_value_h
#define clox_value_h

#include "./common.h"

typedef enum {
	VAL_BOOL,
	VAL_NIL,
	VAL_NUMBER,
} ValueType;

// compiler will add padding after the 4 byte tag to align our 8 byte double
//
// Note that 16 bytes per value is pretty large, we can optimize this later
typedef struct {
	ValueType type;
	union {
	  bool boolean;
	  double number;
	} as;
} Value;

// checks the type of a Value
#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NIL(value) ((value).type == VAL_NIL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)

// handles conversion from clox types to C types
#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)

// handles conversion from C types to clox types
#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL ((Value){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})





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